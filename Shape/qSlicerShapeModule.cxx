/*==============================================================================

  Copyright (c) The Intervention Centre
  Oslo University Hospital, Oslo, Norway. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Rafael Palomar (The Intervention Centre,
  Oslo University Hospital) and was supported by The Research Council of Norway
  through the ALive project (grant nr. 311393).

==============================================================================*/

#include "qSlicerShapeModule.h"


// Qt includes
#include <QDebug>

// Shape Logic includes
#include "vtkSlicerShapeLogic.h"

// Shape Widgets includes
#include "qMRMLMarkupsShapeWidget.h"

// Markups Widgets includes
#include "qMRMLMarkupsOptionsWidgetsFactory.h"

#include <qSlicerModuleManager.h>
#include <qSlicerApplication.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerShapeModulePrivate
{
public:
  qSlicerShapeModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerShapeModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerShapeModulePrivate::qSlicerShapeModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerShapeModule methods

//-----------------------------------------------------------------------------
qSlicerShapeModule::qSlicerShapeModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerShapeModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerShapeModule::~qSlicerShapeModule()
{
}

//-----------------------------------------------------------------------------
bool qSlicerShapeModule::isHidden() const
{
  // The module has no GUI.
  // Widget options will be shown in Markups module.
  return true;
}

//-----------------------------------------------------------------------------
QString qSlicerShapeModule::helpText() const
{
  return "This module contains a markups node named 'Shape'; it is drawn as different types of geometry.";
}

//-----------------------------------------------------------------------------
QString qSlicerShapeModule::acknowledgementText() const
{
  return "This work has been partially funded by The Research Council of Norway (grant nr. 311393)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerShapeModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Rafael Palomar (Oslo University Hospital / NTNU)");
  moduleContributors << QString("Saleem Edah-Tally [Surgeon] [Hobbyist developer]");
  moduleContributors << QString("Andras Lasso (PerkLab)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerShapeModule::icon() const
{
  return QIcon(":/Icons/Shape.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerShapeModule::categories() const
{
  return QStringList() << "Testing.TestCases";
}

//-----------------------------------------------------------------------------
QStringList qSlicerShapeModule::dependencies() const
{
  return QStringList() << "Markups";
}

//-----------------------------------------------------------------------------
void qSlicerShapeModule::setup()
{
  this->Superclass::setup();

  // Create and configure the options widgets
  auto optionsWidgetFactory = qMRMLMarkupsOptionsWidgetsFactory::instance();
  optionsWidgetFactory->registerOptionsWidget(new qMRMLMarkupsShapeWidget());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerShapeModule::createWidgetRepresentation()
{
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerShapeModule::createLogic()
{
  return vtkSlicerShapeLogic::New();
}
