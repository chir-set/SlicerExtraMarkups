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

#include "qSlicerSphereModule.h"


// Qt includes
#include <QDebug>

// Sphere Logic includes
#include "vtkSlicerSphereLogic.h"

// Sphere Widgets includes
#include "qMRMLMarkupsSphereWidget.h"

// Markups Widgets includes
#include "qMRMLMarkupsOptionsWidgetsFactory.h"

#include <qSlicerModuleManager.h>
#include <qSlicerApplication.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerSphereModulePrivate
{
public:
  qSlicerSphereModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerSphereModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerSphereModulePrivate::qSlicerSphereModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerSphereModule methods

//-----------------------------------------------------------------------------
qSlicerSphereModule::qSlicerSphereModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerSphereModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerSphereModule::~qSlicerSphereModule()
{
}

//-----------------------------------------------------------------------------
bool qSlicerSphereModule::isHidden() const
{
  // The module has no GUI.
  // Widget options will be shown in Markups module.
  return true;
}

//-----------------------------------------------------------------------------
QString qSlicerSphereModule::helpText() const
{
  return "This module contains fundamental markups to be used in the Slicer-Liver extension.";
}

//-----------------------------------------------------------------------------
QString qSlicerSphereModule::acknowledgementText() const
{
  return "This work has been partially funded by The Research Council of Norway (grant nr. 311393)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSphereModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Rafael Palomar (Oslo University Hospital / NTNU)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerSphereModule::icon() const
{
  return QIcon(":/Icons/Sphere.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerSphereModule::categories() const
{
  return QStringList() << "Testing.TestCases";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSphereModule::dependencies() const
{
  return QStringList() << "Markups";
}

//-----------------------------------------------------------------------------
void qSlicerSphereModule::setup()
{
  this->Superclass::setup();
  // Create and configure the options widgets
  auto optionsWidgetFactory = qMRMLMarkupsOptionsWidgetsFactory::instance();
  optionsWidgetFactory->registerOptionsWidget(new qMRMLMarkupsSphereWidget());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerSphereModule::createWidgetRepresentation()
{
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerSphereModule::createLogic()
{

  return vtkSlicerSphereLogic::New();
}
