#pragma once
#include "stdafx.h"
#include <windows.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXYPlotActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include "math\BasicMath.h"

class vtkShowHistogramTimerCallback : public vtkCommand
{
public:
	double_vector2d m_data;
	vtkXYPlotActor_Sptr m_plot;
	vtkRenderWindow_Sptr m_renderWindow;
	vtkRenderWindowInteractor_Sptr m_interactor;
	bool m_Stop;
	HANDLE m_hMutex;
	void Lock()
	{
		WaitForSingleObject(m_hMutex, 0);
	}
	void Unlock()
	{
		ReleaseMutex(m_hMutex);
	}
	static vtkShowHistogramTimerCallback* New();
	void Stop();
	virtual void Execute(vtkObject* vtkNotUsed(caller), unsigned long eventId,
		void* vtkNotUsed(callData));
};
VTK_SMART_POINTER(vtkShowHistogramTimerCallback);
