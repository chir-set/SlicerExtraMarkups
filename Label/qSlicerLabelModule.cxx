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

#include "qSlicerLabelModule.h"


// Qt includes
#include <QDebug>

// Label Logic includes
#include "vtkSlicerLabelLogic.h"

// Label Widgets includes
#include "qMRMLMarkupsLabelWidget.h"

// Markups Widgets includes
#include "qMRMLMarkupsOptionsWidgetsFactory.h"

#include <qSlicerModuleManager.h>
#include <qSlicerApplication.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLabelModulePrivate
{
public:
  qSlicerLabelModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerLabelModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerLabelModulePrivate::qSlicerLabelModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerLabelModule methods

//-----------------------------------------------------------------------------
qSlicerLabelModule::qSlicerLabelModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerLabelModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerLabelModule::~qSlicerLabelModule()
{
}

//-----------------------------------------------------------------------------
bool qSlicerLabelModule::isHidden() const
{
  // The module has no GUI.
  // Widget options will be shown in Markups module.
  return true;
}

//-----------------------------------------------------------------------------
QString qSlicerLabelModule::helpText() const
{
  return "This module contains a markups node named 'Label'; it is drawn as an arrow.";
}

//-----------------------------------------------------------------------------
QString qSlicerLabelModule::acknowledgementText() const
{
  return "This work has been partially funded by The Research Council of Norway (grant nr. 311393)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLabelModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Rafael Palomar (Oslo University Hospital / NTNU)");
  moduleContributors << QString("Saleem Edah-Tally [Surgeon] [Hobbyist developer]");
  moduleContributors << QString("Andras Lasso (PerkLab)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerLabelModule::icon() const
{
  return QIcon(":/Icons/Label.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerLabelModule::categories() const
{
  return QStringList() << "Testing.TestCases";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLabelModule::dependencies() const
{
  return QStringList() << "Markups";
}

//-----------------------------------------------------------------------------
void qSlicerLabelModule::setup()
{
  this->Superclass::setup();

  // Create and configure the options widgets
  auto optionsWidgetFactory = qMRMLMarkupsOptionsWidgetsFactory::instance();
  optionsWidgetFactory->registerOptionsWidget(new qMRMLMarkupsLabelWidget());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerLabelModule::createWidgetRepresentation()
{
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerLabelModule::createLogic()
{
  return vtkSlicerLabelLogic::New();
}
