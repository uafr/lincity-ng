/* ---------------------------------------------------------------------- *
 * src/lincity/modules/windmill.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "windmill.h"

#include <list>                     // for _List_iterator

#include "modules.h"


WindmillConstructionGroup windmillConstructionGroup(
    N_("Windmill"),
     FALSE,                     /* need credit? */
     GROUP_WINDMILL,
     GROUP_WINDMILL_SIZE,
     GROUP_WINDMILL_COLOUR,
     GROUP_WINDMILL_COST_MUL,
     GROUP_WINDMILL_BUL_COST,
     GROUP_WINDMILL_FIREC,
     GROUP_WINDMILL_COST,
     GROUP_WINDMILL_TECH,
     GROUP_WINDMILL_RANGE
);

Construction *WindmillConstructionGroup::createConstruction() {
  return new Windmill(this);
}

void Windmill::update()
{
    if (!(total_time%(WINDMILL_RCOST)))
    {   windmill_cost++;}
    int lovolt_made = (commodityCount[STUFF_LOVOLT] + lovolt_output <= MAX_LOVOLT_AT_WINDMILL)?lovolt_output:MAX_LOVOLT_AT_WINDMILL-commodityCount[STUFF_LOVOLT];
    int labor_used = WINDMILL_LABOR * lovolt_made / lovolt_output;

    if ((commodityCount[STUFF_LABOR] >= labor_used)
     && (lovolt_made >= WINDMILL_LOVOLT))
    {
        consumeStuff(STUFF_LABOR, labor_used);
        produceStuff(STUFF_LOVOLT, lovolt_made);
        animate_enable = true;
        working_days += lovolt_made;
    }
    else
    {   animate_enable = false;}
    //monthly update
    if (total_time % 100 == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void Windmill::animate() {
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(ANTIQUE_WINDMILL_ANIM_SPEED);
    ++frameIt->frame %= 3;
  }
}

void Windmill::report()
{
    int i = 0;
    mps_store_title(i, constructionGroup->name);
    mps_store_sfp(i++, N_("busy"), float(busy) / lovolt_output);
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, N_("Output"), lovolt_output);
    // i++;
    list_commodities(&i);
}

void Windmill::place(int x, int y) {
  Construction::place(x, y);

  this->lovolt_output = (int)(WINDMILL_LOVOLT +
    (((double)tech * WINDMILL_LOVOLT) / MAX_TECH_LEVEL));

  commodityMaxProd[STUFF_LOVOLT] = 100 * lovolt_output;
}

void Windmill::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech", "%d", tech);
  Construction::save(xmlWriter);
}

bool Windmill::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = std::stoi(xmlReader.read_inner_xml());
  else if(name == "kwh_output");
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/windmill.cpp */
