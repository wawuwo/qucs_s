/***************************************************************************
                          cccs.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cccs.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"
#include "extsimkernels/verilogawriter.h"


CCCS::CCCS()
{
  Description = QObject::tr("current controlled current source");

  Arcs.append(new qucs::Arc(0,-11, 22, 22,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11, -7, 11,  7,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line( 11,  6, 15,  1,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line( 11,  6,  7,  1,QPen(Qt::darkBlue,3)));

  Lines.append(new qucs::Line(-30,-30,-12,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 30,-12, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11, 30, 30, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-12,-30,-12, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11,-30, 11,-11,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11, 30, 11, 11,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-12, 20,-17, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12, 20, -8, 11,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-25,-27, 25,-27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 25,-27, 25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 25, 27,-25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line(-25, 27,-25,-27,QPen(Qt::darkGray,1)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "CCCS";
  Name  = "SRC";
  SpiceModel = "F";

  Props.append(new Property("G", "1", true,
		QObject::tr("forward transfer factor")));
  Props.append(new Property("T", "0", false, QObject::tr("delay time")));
}

CCCS::~CCCS()
{
}

Component* CCCS::newOne()
{
  return new CCCS();
}

Element* CCCS::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Current Controlled Current Source");
  BitmapFile = (char *) "cccs";

  if(getNewOne)  return new CCCS();
  return 0;
}

QString CCCS::va_code()
{  
    
    QString Gain = vacompat::normalize_value(Props.at(0)->Value); 
	QString P1 = Ports.at(0)->Connection->Name;
    QString P4 = Ports.at(1)->Connection->Name;
    QString P3 = Ports.at(2)->Connection->Name;
    QString P2 = Ports.at(3)->Connection->Name;
    QString s = "";
    
    QString Vpm = vacompat::normalize_voltage(P1,P2);
    QString Ipm = vacompat::normalize_current(P1,P2,true);  
    s += QString(" %1  <+  %2 * 1e3;\n").arg(Ipm).arg(Vpm);
    QString Vpm2 = vacompat::normalize_voltage(P4,P3);
    QString Ipm2 = vacompat::normalize_current(P4,P3,true); 
    s += QString("%1  <+   %2 * 1e-9;\n").arg(Ipm2).arg(Vpm2);
    s += QString("%1  <+   %2 * 1e3 * %3 ;\n").arg(Ipm2).arg(Vpm).arg(Gain);
    
    return s;
}
QString CCCS::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel); // spice CCCS consists two sources: output source
                        // and zero value controlling source
    QString val = spicecompat::normalize_value(Props.at(0)->Value);
    s += QString(" %1 %2 ").arg(Ports.at(1)->Connection->Name)
            .arg(Ports.at(2)->Connection->Name); // output source nodes
    s.replace(" gnd ", " 0 ");
    s += QString(" V%1 %2\n").arg(Name).arg(val);
    s += QString("V%1 %2 %3 DC 0\n").arg(Name).arg(Ports.at(0)->Connection->Name)
            .arg(Ports.at(3)->Connection->Name);   // controlling 0V source

    return s;
}
