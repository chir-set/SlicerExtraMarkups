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

#include "qSlicerRingModule.h"


// Qt includes
#include <QDebug>

// Ring Logic includes
#include "vtkSlicerRingLogic.h"

// Ring Widgets includes
#include "qMRMLMarkupsRingWidget.h"

// Markups Widgets includes
#include "qMRMLMarkupsOptionsWidgetsFactory.h"

#include <qSlicerModuleManager.h>
#include <qSlicerApplication.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerRingModulePrivate
{
public:
  qSlicerRingModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerRingModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerRingModulePrivate::qSlicerRingModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerRingModule methods

//-----------------------------------------------------------------------------
qSlicerRingModule::qSlicerRingModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerRingModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerRingModule::~qSlicerRingModule()
{
}

//-----------------------------------------------------------------------------
bool qSlicerRingModule::isHidden() const
{
  // The module has no GUI.
  // Widget options will be shown in Markups module.
  return true;
}

//-----------------------------------------------------------------------------
QString qSlicerRingModule::helpText() const
{
  return "This module contains a derived markup that draws a ring using a base markups line.";
}

//-----------------------------------------------------------------------------
QString qSlicerRingModule::acknowledgementText() const
{
  return "This work has been partially funded by The Research Council of Norway (grant nr. 311393)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerRingModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Rafael Palomar (Oslo University Hospital / NTNU)");
  moduleContributors << QString("SET [Surgeon] [Hobbyist developper]");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerRingModule::icon() const
{
  return QIcon(":/Icons/Ring.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerRingModule::categories() const
{
  return QStringList() << "Testing.TestCases";
}

//-----------------------------------------------------------------------------
QStringList qSlicerRingModule::dependencies() const
{
  return QStringList() << "Markups";
}

//-----------------------------------------------------------------------------
void qSlicerRingModule::setup()
{
  this->Superclass::setup();

  // This is a test class, therefore we do not register anything if
  // not in testing mode (to avoid cluttering the markups module).
  /*
  bool isTestingEnabled = qSlicerApplication::testAttribute(qSlicerCoreApplication::AA_EnableTesting);
  if (!isTestingEnabled)
    {
    return;
    }
  */
  // Create and configure the options widgets
  auto optionsWidgetFactory = qMRMLMarkupsOptionsWidgetsFactory::instance();
  optionsWidgetFactory->registerOptionsWidget(new qMRMLMarkupsRingWidget());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerRingModule::createWidgetRepresentation()
{
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerRingModule::createLogic()
{
  // This is a test class, therefore we do not register anything (to avoid cluttering the markups module)
  // unless the application is in testing mode.
  /*
  bool isTestingEnabled = qSlicerApplication::testAttribute(qSlicerCoreApplication::AA_EnableTesting);
  if (!isTestingEnabled)
    {
    return nullptr;
    }
  */
  return vtkSlicerRingLogic::New();
}
