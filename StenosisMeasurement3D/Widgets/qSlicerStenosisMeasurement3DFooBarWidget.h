/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerStenosisMeasurement3DFooBarWidget_h
#define __qSlicerStenosisMeasurement3DFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerStenosisMeasurement3DModuleWidgetsExport.h"

class qSlicerStenosisMeasurement3DFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_StenosisMeasurement3D
class Q_SLICER_MODULE_STENOSISMEASUREMENT3D_WIDGETS_EXPORT qSlicerStenosisMeasurement3DFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerStenosisMeasurement3DFooBarWidget(QWidget *parent=0);
  ~qSlicerStenosisMeasurement3DFooBarWidget() override;

protected slots:

protected:
  QScopedPointer<qSlicerStenosisMeasurement3DFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerStenosisMeasurement3DFooBarWidget);
  Q_DISABLE_COPY(qSlicerStenosisMeasurement3DFooBarWidget);
};

#endif
