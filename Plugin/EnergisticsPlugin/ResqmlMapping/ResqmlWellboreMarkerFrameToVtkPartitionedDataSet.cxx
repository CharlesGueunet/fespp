/*-----------------------------------------------------------------------
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"; you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
-----------------------------------------------------------------------*/
#include "ResqmlMapping/ResqmlWellboreMarkerFrameToVtkPartitionedDataSet.h"

#include <iostream>
#include <string>

// include VTK library
#include <vtkInformation.h>

#include <fesapi/resqml2/WellboreMarker.h>
#include <fesapi/resqml2/WellboreMarkerFrameRepresentation.h>
#include <fesapi/resqml2/AbstractLocal3dCrs.h>

#include "ResqmlMapping/ResqmlWellboreMarkerToVtkPolyData.h"

//----------------------------------------------------------------------------
ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::ResqmlWellboreMarkerFrameToVtkPartitionedDataSet(RESQML2_NS::WellboreMarkerFrameRepresentation *marker_frame, bool orientation, int size, int proc_number, int max_proc)
	: ResqmlAbstractRepresentationToVtkDataset(marker_frame,
											   proc_number - 1,
											   max_proc),
	  orientation(orientation),
	  size(size),
	  resqmlData(marker_frame)
{
	this->vtkData = vtkSmartPointer<vtkPartitionedDataSet>::New();
	this->vtkData->Modified();
}

//----------------------------------------------------------------------------
void ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::loadVtkObject()
{
	for (int idx = 0; idx < this->list_marker.size(); ++idx) 
	{
<<<<<<< HEAD
		this->vtkData->SetPartition(idx, this->list_marker[idx]->getOutput()->GetPartitionAsDataObject(0));
		this->vtkData->GetMetaData(idx)->Set(vtkCompositeDataSet::NAME(), this->list_marker[idx]->getTitle().c_str());
=======
		if (markerSet[mIndex]->getUuid() == this->uuid)
		{
			std::unique_ptr<double[]> doublePositions(new double[this->resqmlData->getMdValuesCount() * 3]);
			this->resqmlData->getXyzPointsOfPatchInGlobalCrs(0, doublePositions.get());

			if (orientation)
			{
				if (!std::isnan(doublePositions[3 * mIndex]) &&
					!std::isnan(doublePositions[3 * mIndex + 1]) &&
					!std::isnan(doublePositions[3 * mIndex + 2]))
				{ // no NaN Value
					if (markerSet[mIndex]->hasDipAngle() &&
						markerSet[mIndex]->hasDipDirection())
					{ // dips & direction exist
						createDisk(mIndex);
					}
					else
					{
						createSphere(mIndex);
					}
				}
			}
			else
			{
				createSphere(mIndex);
			}
		}
>>>>>>> 87b6c1885de0d2ecf761379408749906bfb7e3c8
	}
}

//----------------------------------------------------------------------------
void ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::toggleMarkerOrientation(bool orient)
{
	orientation = orient;
}

//----------------------------------------------------------------------------
void ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::setMarkerSize(int new_size)
{
	size = new_size;
}

//----------------------------------------------------------------------------
void ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::addMarker(std::string marker_uuid)
{
	bool exist = false;
	for (int idx = 0; idx < this->list_marker.size(); ++idx) 
	{
		if (this->list_marker[idx]->getUuid() == marker_uuid)
		{
			exist = true;
		}
	}
	if (!exist)
	{
		this->list_marker.push_back(new ResqmlWellboreMarkerToVtkPolyData(this->resqmlData, marker_uuid, this->orientation, this->size));
		this->loadVtkObject();
	}
}

//----------------------------------------------------------------------------
<<<<<<< HEAD
void ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::removeMarker(std::string marker_uuid)
{
	for (auto it = this->list_marker.begin(); it != this->list_marker.end();)
	{
		ResqmlWellboreMarkerToVtkPolyData *marker = *it;
		if (marker->getUuid() == marker_uuid)
		{
			it = this->list_marker.erase(it);
		}
		else
		{
			++it;
		}
	}
	this->loadVtkObject();
=======
void ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::createDisk(unsigned int markerIndex)
{
	vtkSmartPointer<vtkPolyData> vtkPolydata = vtkSmartPointer<vtkPolyData>::New();

	std::unique_ptr<double[]> doublePositions(new double[this->resqmlData->getMdValuesCount() * 3]);
	this->resqmlData->getXyzPointsOfPatchInGlobalCrs(0, doublePositions.get());

	// initialize a disk
	vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
	diskSource->SetInnerRadius(0);
	diskSource->SetOuterRadius(size);
	diskSource->Update();

	vtkPolydata = diskSource->GetOutput();

	// get markerSet
	std::vector<RESQML2_NS::WellboreMarker *> markerSet = this->resqmlData->getWellboreMarkerSet();

	// disk orientation with dipAngle & dip Direction
	vtkSmartPointer<vtkTransform> rotation = vtkSmartPointer<vtkTransform>::New();
	double dipDirectionInDegree = convertToDegree(markerSet[markerIndex]->getDipDirectionValue(), markerSet[markerIndex]->getDipDirectionUom());
	rotation->RotateZ(-dipDirectionInDegree);
	double dipAngleInDegree = convertToDegree(markerSet[markerIndex]->getDipAngleValue(), markerSet[markerIndex]->getDipAngleUom());
	rotation->RotateX(-dipAngleInDegree);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(vtkPolydata);
	transformFilter->SetTransform(rotation);
	transformFilter->Update();

	vtkPolydata = transformFilter->GetOutput();

	// disk translation with marker position
	const double zIndice = this->resqmlData->getLocalCrs(0)->isDepthOriented() ? -1 : 1;
	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	translation->Translate(doublePositions[3 * markerIndex], doublePositions[3 * markerIndex + 1], zIndice * doublePositions[3 * markerIndex + 2]);

	transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(vtkPolydata);
	transformFilter->SetTransform(translation);
	transformFilter->Update();

	this->vtkData->SetPartition(markerIndex, transformFilter->GetOutput());
}

//----------------------------------------------------------------------------
void ResqmlWellboreMarkerFrameToVtkPartitionedDataSet::createSphere(unsigned int markerIndex)
{
	std::unique_ptr<double[]> doublePositions(new double[this->resqmlData->getMdValuesCount() * 3]);
	this->resqmlData->getXyzPointsOfPatchInGlobalCrs(0, doublePositions.get());

	// get markerSet
	const double zIndice = this->resqmlData->getLocalCrs(0)->isDepthOriented() ? -1 : 1;

	// create  sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(doublePositions[3 * markerIndex], doublePositions[3 * markerIndex + 1], zIndice * doublePositions[3 * markerIndex + 2]);
	sphereSource->SetRadius(size);
	sphereSource->Update();

	this->vtkData->SetPartition(markerIndex, sphereSource->GetOutput());
>>>>>>> 87b6c1885de0d2ecf761379408749906bfb7e3c8
}
