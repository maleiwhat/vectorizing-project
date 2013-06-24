
#include "vtkShowHistogramTimerCallback.h"

vtkShowHistogramTimerCallback* vtkShowHistogramTimerCallback::New()
{
	vtkShowHistogramTimerCallback* cb = new vtkShowHistogramTimerCallback;
	for ( int i=0;i<8;i++)
	{
		cb->m_data.push_back(double_vector());
		cb->m_data[i].push_back(0);
	}
	cb->m_hMutex = CreateMutex(
					   NULL,              // default security attributes
					   FALSE,             // initially not owned
					   NULL);             // unnamed mutex
	cb->m_Stop = false;
	return cb;
}

void vtkShowHistogramTimerCallback::Execute(vtkObject* (caller),
		unsigned long eventId, void* vtkNotUsed(callData))
{
	Lock();
	m_plot->RemoveAllInputs();
	for (unsigned int i = 0 ; i < m_data.size() ; i++)
	{
		vtkSmartPointer<vtkDoubleArray> array_s =
			vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkFieldData> field =
			vtkSmartPointer<vtkFieldData>::New();
		vtkSmartPointer<vtkDataObject> data =
			vtkSmartPointer<vtkDataObject>::New();
		for (int b = 0; b < m_data[i].size(); b++)
		{
			array_s->InsertValue(b, m_data[i][b]);
		}
		field->AddArray(array_s);
		data->SetFieldData(field);
		m_plot->AddDataObjectInput(data);
	}
	m_renderWindow->Render();
	if (m_Stop)
	{
		vtkRenderWindowInteractor* iren =
			static_cast<vtkRenderWindowInteractor*>(caller);
		// Close the window
		iren->GetRenderWindow()->Finalize();
		// Stop the interactor
		iren->TerminateApp();
	}
	Unlock();
}

void vtkShowHistogramTimerCallback::Stop()
{
	m_Stop = true;
}

vtkShowHistogramTimerCallback::~vtkShowHistogramTimerCallback()
{
	CloseHandle(m_hMutex);
}
