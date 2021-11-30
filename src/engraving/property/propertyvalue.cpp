/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "propertyvalue.h"

#include "realfn.h"

#include "libmscore/groups.h"

#include "log.h"

using namespace mu::engraving;

PropertyValue::PropertyValue(const Ms::Groups& v)
    : m_type(P_TYPE::GROUPS), m_any(v)
{
}

const Ms::Groups& PropertyValue::toGroups() const
{
    return std::any_cast<const Ms::Groups&>(m_any);
}

PropertyValue::PropertyValue(const Ms::TDuration& v)
    : m_type(P_TYPE::TDURATION), m_any(v)
{
}

const Ms::TDuration& PropertyValue::toTDuration() const
{
    return std::any_cast<const Ms::TDuration&>(m_any);
}

bool PropertyValue::isValid() const
{
    return m_type != P_TYPE::UNDEFINED;
}

P_TYPE PropertyValue::type() const
{
    return m_type;
}

bool PropertyValue::operator ==(const PropertyValue& v) const
{
    if (v.m_type == P_TYPE::UNDEFINED || m_type == P_TYPE::UNDEFINED) {
        return v.m_type == m_type;
    }

    //! HACK Temporary hack for bool comparisons (maybe one type is bool and another type is int)
    if (v.m_type == P_TYPE::BOOL || m_type == P_TYPE::BOOL) {
        return v.value<bool>() == value<bool>();
    }

    //! HACK Temporary hack for int comparisons (maybe one type is int and another type is enum)
    if (v.m_type == P_TYPE::INT || m_type == P_TYPE::INT) {
        return v.value<int>() == value<int>();
    }

    //! HACK Temporary hack for Spatium comparisons (maybe one type is Spatium and another type is real)
    if (v.m_type == P_TYPE::SPATIUM || m_type == P_TYPE::SPATIUM) {
        return RealIsEqual(v.value<qreal>(), value<qreal>());
    }

    //! HACK Temporary hack for Fraction comparisons
    if (v.m_type == P_TYPE::FRACTION) {
        assert(m_type == P_TYPE::FRACTION);
        return v.value<Fraction>().identical(value<Fraction>());
    }

    //! HACK Temporary hack for TDURATION comparisons
    if (v.m_type == P_TYPE::TDURATION) {
        assert(m_type == P_TYPE::TDURATION);
        return v.toTDuration() == toTDuration();
    }

    //! HACK Temporary hack for GROUPS comparisons
    if (v.m_type == P_TYPE::GROUPS) {
        assert(m_type == P_TYPE::GROUPS);
        return v.toGroups() == toGroups();
    }

    if (v.m_type == P_TYPE::REAL) {
        assert(m_type == P_TYPE::REAL);
        return RealIsEqual(v.value<qreal>(), value<qreal>());
    }

    assert(m_data);
    if (!m_data) {
        return false;
    }

    assert(v.m_data);
    if (!v.m_data) {
        return false;
    }

    return v.m_type == m_type && v.m_data->equal(m_data.get());
}

QVariant PropertyValue::toQVariant() const
{
    switch (m_type) {
    case P_TYPE::UNDEFINED:   return QVariant();
    // Base
    case P_TYPE::BOOL:        return value<bool>();
    case P_TYPE::INT:         return value<int>();
    case P_TYPE::REAL:        return value<qreal>();
    case P_TYPE::STRING:      return value<QString>();
    // Geometry
    case P_TYPE::POINT:       return value<PointF>().toQPointF();
    case P_TYPE::SIZE:        return value<SizeF>().toQSizeF();
    case P_TYPE::PATH: {
        UNREACHABLE; //! TODO
    }
    break;
    case P_TYPE::SCALE: {
        UNREACHABLE; //! TODO
    }
    break;
    case P_TYPE::SPATIUM:     return value<Spatium>().val();
    case P_TYPE::MILLIMETRE:  return qreal(value<Millimetre>());
    case P_TYPE::PAIR_REAL:   return QVariant::fromValue(value<PairF>().toQPairF());

    // Draw
    case P_TYPE::COLOR:       return value<draw::Color>().toQColor();
    case P_TYPE::ORNAMENT_STYLE: return static_cast<int>(value<OrnamentStyle>());
    case P_TYPE::GLISS_STYLE: return static_cast<int>(value<GlissandoStyle>());

    // Layout
    case P_TYPE::ALIGN:       return static_cast<int>(value<Align>());
    case P_TYPE::PLACEMENT_V: return static_cast<int>(value<PlacementV>());
    case P_TYPE::PLACEMENT_H: return static_cast<int>(value<PlacementH>());
    case P_TYPE::DIRECTION_V: return static_cast<int>(value<DirectionV>());
    case P_TYPE::DIRECTION_H: return static_cast<int>(value<DirectionH>());
    case P_TYPE::BEAM_MODE:   return static_cast<int>(value<BeamMode>());

    // Duration
    case P_TYPE::FRACTION:    return QVariant::fromValue(value<Fraction>().toString());
    case P_TYPE::TDURATION:   return QVariant::fromValue(toTDuration());

    // Types
    case P_TYPE::LAYOUTBREAK_TYPE: return static_cast<int>(value<LayoutBreakType>());
    case P_TYPE::VELO_TYPE:        return static_cast<int>(value<VeloType>());
    case P_TYPE::BARLINE_TYPE:     return static_cast<int>(value<BarLineType>());
    case P_TYPE::NOTEHEAD_TYPE:    return static_cast<int>(value<NoteHeadType>());

    // other
    case P_TYPE::SYMID:           return static_cast<int>(value<Ms::SymId>());
    case P_TYPE::HOOK_TYPE:       return static_cast<int>(value<Ms::HookType>());
    case P_TYPE::DYNAMIC_TYPE:    return static_cast<int>(value<Ms::DynamicType>());
    case P_TYPE::ACCIDENTAL_ROLE: return static_cast<int>(value<Ms::AccidentalRole>());
    default:
        UNREACHABLE; //! TODO
    }

    return QVariant();
}

PropertyValue PropertyValue::fromQVariant(const QVariant& v, P_TYPE type)
{
    switch (type) {
    case P_TYPE::UNDEFINED:  // try by QVariant type
        break;

    // Base
    case P_TYPE::BOOL:          return PropertyValue(v.toBool());
    case P_TYPE::INT:           return PropertyValue(v.toInt());
    case P_TYPE::REAL:          return PropertyValue(v.toReal());
    case P_TYPE::STRING:        return PropertyValue(v.toString());

    // Geometry
    case P_TYPE::POINT:         return PropertyValue(PointF::fromQPointF(v.value<QPointF>()));
    case P_TYPE::SIZE:          return PropertyValue(SizeF::fromQSizeF(v.value<QSizeF>()));
    case P_TYPE::PATH: {
        UNREACHABLE; //! TODO
    }
    break;
    case P_TYPE::SCALE:         return PropertyValue(ScaleF::fromQSizeF(v.value<QSizeF>()));
    case P_TYPE::SPATIUM:       return PropertyValue(Spatium(v.toReal()));
    case P_TYPE::MILLIMETRE:    return PropertyValue(Millimetre(v.toReal()));
    case P_TYPE::PAIR_REAL:     return PropertyValue(PairF::fromQPairF(v.value<QPair<qreal, qreal> >()));

    // Draw
    case P_TYPE::COLOR:         return PropertyValue(Color::fromQColor(v.value<QColor>()));
    case P_TYPE::ORNAMENT_STYLE: return PropertyValue(OrnamentStyle(v.toInt()));
    case P_TYPE::GLISS_STYLE:   return PropertyValue(GlissandoStyle(v.toInt()));

    // Layout
    case P_TYPE::ALIGN:         return PropertyValue(Align(v.toInt()));
    case P_TYPE::PLACEMENT_V:   return PropertyValue(PlacementV(v.toInt()));
    case P_TYPE::PLACEMENT_H:   return PropertyValue(PlacementH(v.toInt()));
    case P_TYPE::DIRECTION_V:   return PropertyValue(DirectionV(v.toInt()));
    case P_TYPE::DIRECTION_H:   return PropertyValue(DirectionH(v.toInt()));
    case P_TYPE::BEAM_MODE:     return PropertyValue(BeamMode(v.toInt()));

    // Duration
    case P_TYPE::FRACTION:      return PropertyValue(Fraction::fromString(v.toString()));
    case P_TYPE::TDURATION: {
        UNREACHABLE; //! TODO
    }
    break;

    // Types
    case P_TYPE::LAYOUTBREAK_TYPE: return PropertyValue(LayoutBreakType(v.toInt()));
    case P_TYPE::VELO_TYPE:        return PropertyValue(VeloType(v.toInt()));
    case P_TYPE::BARLINE_TYPE:     return PropertyValue(BarLineType(v.toInt()));
    case P_TYPE::NOTEHEAD_TYPE:    return PropertyValue(NoteHeadType(v.toInt()));

    // other

    case P_TYPE::SYMID:        return PropertyValue(Ms::SymId(v.toInt()));
    case P_TYPE::HOOK_TYPE:    return PropertyValue(Ms::HookType(v.toInt()));
    case P_TYPE::DYNAMIC_TYPE: return PropertyValue(Ms::DynamicType(v.toInt()));
    case P_TYPE::ACCIDENTAL_ROLE: return PropertyValue(Ms::AccidentalRole(v.toInt()));
    default:
        break;
    }

    //! NOTE Try determinate type by QVariant type
    switch (v.type()) {
    case QVariant::Invalid:     return PropertyValue();
    case QVariant::Bool:        return PropertyValue(v.toBool());
    case QVariant::Int:         return PropertyValue(v.toInt());
    case QVariant::UInt:        return PropertyValue(v.toInt());
    case QVariant::LongLong:    return PropertyValue(v.toInt());
    case QVariant::ULongLong:   return PropertyValue(v.toInt());
    case QVariant::Double:      return PropertyValue(v.toReal());
    case QVariant::Char:        return PropertyValue(v.toInt());
    case QVariant::String:      return PropertyValue(v.toString());
    case QVariant::Size:        return PropertyValue(SizeF::fromQSizeF(QSizeF(v.toSize())));
    case QVariant::SizeF:       return PropertyValue(SizeF::fromQSizeF(v.toSizeF()));
    case QVariant::Point:       return PropertyValue(PointF::fromQPointF(QPointF(v.toPoint())));
    case QVariant::PointF:      return PropertyValue(PointF::fromQPointF(v.toPointF()));
    case QVariant::Color:       return PropertyValue(draw::Color::fromQColor(v.value<QColor>()));
    default:
        break;
    }

    UNREACHABLE;
    return PropertyValue();
}