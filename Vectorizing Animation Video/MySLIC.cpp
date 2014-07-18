#include "MySLIC.h"

#include <iostream>
#include <windows.h>
#include <cmath>
#undef min
#undef max

MySLIC::MySLIC(const cv::Mat& img)
{
	///RGB轉UINT  因SLIC中所使用的顏色為ARGB，故先將原圖形做轉換後存至img以利計算
	int size = img.rows * img.cols;
	m_width = img.cols;
	m_height = img.rows;
	m_Buff.resize(size);
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			cv::Vec3b salColor = img.at<cv::Vec3b>(j, i);   //CV_IMAGE_ELEM(image,element type,row,col)
			m_Buff[j * m_width + i] = salColor[0] + salColor[1] * 256 + salColor[2] * 256 * 256;
			// B => salColor[0] ; G => salColor[1] ; R => salColor[2]
		}
	}
	unsigned int* ptr = &m_Buff[0];
	DoRGBtoLABConversion((const unsigned int*&)ptr, m_lvec, m_avec, m_bvec);
}


MySLIC::~MySLIC(void)
{
}

//===========================================================================
/// DoSuperpixelSegmentation_ForGivenSuperpixelSize
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void MySLIC::DoSuperpixelSegmentation_ForGivenSuperpixelSize(
	int*&                       labels_map,
	SLICLabels&                 labels,
	int&                        numlabels,
	const int&                  superpixelsize,
	const double&               compactness)
{
	//------------------------------------------------
	const int STEP = sqrt(double(superpixelsize)) + 0.5;
	//------------------------------------------------
	std::vector<double> kseedsl(0);
	std::vector<double> kseedsa(0);
	std::vector<double> kseedsb(0);
	std::vector<double> kseedsgx(0);
	std::vector<double> kseedsgy(0);
	std::vector<double> kseedsx(0);
	std::vector<double> kseedsy(0);
	//--------------------------------------------------
	int sz = m_width * m_height;
	//--------------------------------------------------
	labels_map = new int[sz];
	for (int s = 0; s < sz; s++)
	{
		labels_map[s] = -1;
	}
	//--------------------------------------------------
	//--------------------------------------------------
	GetSeeds_ForGivenStepSize(kseedsl, kseedsa, kseedsb, kseedsgx, kseedsgy, kseedsx, kseedsy, STEP);
	PerformSuperpixel(kseedsl, kseedsa, kseedsb, kseedsgx, kseedsgy, kseedsx, kseedsy, labels_map, STEP,
					  compactness);
	numlabels = kseedsx.size();
	int* nlabels = new int[sz];
	//Remap
	std::vector<int> map;
	EnforceLabelConnectivity(labels_map, nlabels, map, numlabels, double(sz) / double(STEP * STEP));
	labels.resize(numlabels);
	for (int i = 0; i < sz; i++)
	{
		Pixel p;
		p.x = i % m_width;
		p.y = i / m_width;
		labels[nlabels[i]].index = nlabels[i];
		labels[nlabels[i]].pixels.push_back(p);
		labels_map[i] = nlabels[i];
	}
	for (int i = 0; i < map.size(); i++)
	{
		labels[i].seed_xy.x     = kseedsx[map[i]];
		labels[i].seed_xy.y     = kseedsy[map[i]];
		labels[i].seed_lab.l    = kseedsl[map[i]];
		labels[i].seed_lab.a    = kseedsa[map[i]];
		labels[i].seed_lab.b    = kseedsb[map[i]];
		labels[i].seed_Gx       = kseedsgx[map[i]];
		labels[i].seed_Gy       = kseedsgy[map[i]];
	}
	if (nlabels)
	{
		delete [] nlabels;
	}
}

//===========================================================================
/// DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void MySLIC::DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(
	int*&                       labels_map,
	SLICLabels&                 labels,
	int&                        numlabels,
	const int&                  K,          //required number of superpixels
	const double&               compactness)//weight given to spatial distance
{
	const int superpixelsize = 0.5 + double(m_width * m_height) / double(K);
	DoSuperpixelSegmentation_ForGivenSuperpixelSize(labels_map, labels, numlabels, superpixelsize,
			compactness);
}

//===========================================================================
/// EnforceLabelConnectivity
///
///     1. finding an adjacent label for each new component at the start
///     2. if a certain component is too small, assigning the previously found
///         adjacent label to this component, and not incrementing the label.
//===========================================================================
void MySLIC::EnforceLabelConnectivity(
	const int*
	labels,     //input labels that need to be corrected to remove stray labels
	int*&                       nlabels,    //new labels
	std::vector<int>&           map,        //new label map to old one
	int&
	numlabels,  //the number of labels changes in the end if segments are removed
	const int&                  K)          //the number of superpixels desired by the user
{
	const int dx4[4] = { -1,  0,  1,  0};
	const int dy4[4] = { 0, -1,  0,  1};
	const int sz = m_width * m_height;
	const int SUPSZ = sz / K;
	for (int i = 0; i < sz; i++)
	{
		nlabels[i] = -1;
	}
	int label(0);
	int* xvec = new int[sz];
	int* yvec = new int[sz];
	int oindex(0);
	int adjlabel(0);//adjacent label
	for (int j = 0; j < m_height; j++)
	{
		for (int k = 0; k < m_width; k++)
		{
			if (0 > nlabels[oindex])
			{
				nlabels[oindex] = label;
				//--------------------
				// Start a new segment
				//--------------------
				xvec[0] = k;
				yvec[0] = j;
				//-------------------------------------------------------
				// Quickly find an adjacent label for use later if needed
				//-------------------------------------------------------
				{
					for (int n = 0; n < 4; n++)
					{
						int x = xvec[0] + dx4[n];
						int y = yvec[0] + dy4[n];
						if ((x >= 0 && x < m_width) && (y >= 0 && y < m_height))
						{
							int nindex = y * m_width + x;
							if (nlabels[nindex] >= 0)
							{
								adjlabel = nlabels[nindex];    //**改成最多的label
							}
						}
					}
				}
				int count(1);
				int temp;
				for (int c = 0; c < count; c++)
				{
					for (int n = 0; n < 4; n++)
					{
						int x = xvec[c] + dx4[n];
						int y = yvec[c] + dy4[n];
						if ((x >= 0 && x < m_width) && (y >= 0 && y < m_height))
						{
							int nindex = y * m_width + x;
							if (0 > nlabels[nindex] && labels[oindex] == labels[nindex])
							{
								temp = labels[nindex];
								xvec[count] = x;
								yvec[count] = y;
								nlabels[nindex] = label;
								count++;
							}
						}
					}
				}
				//-------------------------------------------------------
				// If segment size is less then a limit, assign an
				// adjacent label found before, and decrement label count.
				//-------------------------------------------------------
				if (count <= SUPSZ >> 2)
				{
					for (int c = 0; c < count; c++)
					{
						int ind = yvec[c] * m_width + xvec[c];
						nlabels[ind] = adjlabel;
					}
				}
				else
				{
					map.push_back(labels[oindex]);
					label++;
				}
			}
			oindex++;
		}
	}
	numlabels = label;
	if (xvec)
	{
		delete [] xvec;
	}
	if (yvec)
	{
		delete [] yvec;
	}
}

void MySLIC::GetSeeds_ForGivenStepSize(
	std::vector<double>& kseedsl,
	std::vector<double>& kseedsa,
	std::vector<double>& kseedsb,
	std::vector<double>& kseedsgx,
	std::vector<double>& kseedsgy,
	std::vector<double>& kseedsx,
	std::vector<double>& kseedsy,
	const int& STEP)
{
	const bool hexgrid = false;
	int numseeds(0);
	int n(0);
	//int xstrips = m_width/STEP;
	//int ystrips = m_height/STEP;
	int xstrips = (0.5 + double(m_width) / double(STEP));
	int ystrips = (0.5 + double(m_height) / double(STEP));
	int xerr = m_width  - STEP * xstrips;
	if (xerr < 0)
	{
		xstrips--;
		xerr = m_width - STEP * xstrips;
	}
	int yerr = m_height - STEP * ystrips;
	if (yerr < 0)
	{
		ystrips--;
		yerr = m_height - STEP * ystrips;
	}
	double xerrperstrip = double(xerr) / double(xstrips);
	double yerrperstrip = double(yerr) / double(ystrips);
	int xoff = STEP / 2;
	int yoff = STEP / 2;
	//-------------------------
	numseeds = xstrips * ystrips;
	//-------------------------
	kseedsl.resize(numseeds);
	kseedsa.resize(numseeds);
	kseedsb.resize(numseeds);
	kseedsgx.resize(numseeds);
	kseedsgy.resize(numseeds);
	kseedsx.resize(numseeds);
	kseedsy.resize(numseeds);
	for (int y = 0; y < ystrips; y++)
	{
		int ye = y * yerrperstrip;
		for (int x = 0; x < xstrips; x++)
		{
			int xe = x * xerrperstrip;
			int seedx = (x * STEP + xoff + xe);
			if (hexgrid)
			{
				seedx = x * STEP + (xoff << (y & 0x1)) + xe;    //for hex grid sampling
				seedx = std::min(m_width - 1, seedx);
			}
			int seedy = (y * STEP + yoff + ye);
			int i = seedy * m_width + seedx;
			kseedsl[n] = m_lvec[i];
			kseedsa[n] = m_avec[i];
			kseedsb[n] = m_bvec[i];
//          kseedsgx[n] = m_Gradient->GradientX(seedx, seedy);
//          kseedsgy[n] = m_Gradient->GradientY(seedx, seedy);
			kseedsx[n] = seedx;
			kseedsy[n] = seedy;
			n++;
		}
	}
}

void MySLIC::PerformSuperpixel(
	std::vector<double>& kseedsl,
	std::vector<double>& kseedsa,
	std::vector<double>& kseedsb,
	std::vector<double>& kseedsgx,
	std::vector<double>& kseedsgy,
	std::vector<double>& kseedsx,
	std::vector<double>& kseedsy,
	int*& klabels, const int& STEP,
	const double& compactness /*= 10.0*/)
{
	int sz = m_width * m_height;
	const int numk = kseedsl.size();
	//----------------
	int offset = STEP;
	//if(STEP < 8) offset = STEP*1.5;//to prevent a crash due to a very small step size
	//----------------
	std::vector<double> clustersize(numk, 0);
	std::vector<double> inv(numk, 0);//to store 1/clustersize[k] values
	std::vector<double> sigmal(numk, 0);
	std::vector<double> sigmaa(numk, 0);
	std::vector<double> sigmab(numk, 0);
	std::vector<double> sigmagx(numk, 0);
	std::vector<double> sigmagy(numk, 0);
	std::vector<double> sigmax(numk, 0);
	std::vector<double> sigmay(numk, 0);
	std::vector<double> distvec(sz, DBL_MAX);
	double invwt = 1.0 / ((STEP / compactness) * (STEP / compactness));
	for (int itr = 0; itr < 10; itr++)
	{
		distvec.assign(sz, DBL_MAX);
		for (int n = 0; n < numk; n++)
		{
			int x1, y1, x2, y2;
			y1 = std::max(0.0,              kseedsy[n] - offset);
			y2 = std::min((double)m_height, kseedsy[n] + offset);
			x1 = std::max(0.0,              kseedsx[n] - offset);
			x2 = std::min((double)m_width,  kseedsx[n] + offset);
			for (int y = y1; y < y2; y++)
			{
				for (int x = x1; x < x2; x++)
				{
					int i = y * m_width + x;
					double dist;
					double distg;
					double distlab;
					double distxy;
					distlab = (m_lvec[i] - kseedsl[n]) * (m_lvec[i] - kseedsl[n]) +
							  (m_avec[i] - kseedsa[n]) * (m_avec[i] - kseedsa[n]) +
							  (m_bvec[i] - kseedsb[n]) * (m_bvec[i] - kseedsb[n]);
//                  distg = (m_Gradient->GradientX(i) - kseedsgx[n]) * (m_Gradient->GradientX(i) - kseedsgx[n]) +
//                          (m_Gradient->GradientY(i) - kseedsgy[n]) * (m_Gradient->GradientY(i) - kseedsgy[n]);
					distxy = (x - kseedsx[n]) * (x - kseedsx[n]) +
							 (y - kseedsy[n]) * (y - kseedsy[n]);
					//------------------------------------------------------------------------
//					std::cout<<invwt<<std::endl;
					dist = (distlab + distxy * invwt);
					//dist = sqrt(dist) + sqrt(distxy*invwt);//this is more exact
					//------------------------------------------------------------------------
					if (dist < distvec[i])
					{
						distvec[i] = dist;
						klabels[i]  = n;
					}
				}
			}
		}
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		//instead of reassigning memory on each iteration, just reset.
		sigmal.assign(numk, 0);
		sigmaa.assign(numk, 0);
		sigmab.assign(numk, 0);
		sigmagx.assign(numk, 0);
		sigmagy.assign(numk, 0);
		sigmax.assign(numk, 0);
		sigmay.assign(numk, 0);
		clustersize.assign(numk, 0);
		//------------------------------------
		//edgesum.assign(numk, 0);
		//------------------------------------
		{
			int ind(0);
			for (int r = 0; r < m_height; r++)
			{
				for (int c = 0; c < m_width; c++)
				{
					sigmal[klabels[ind]] += m_lvec[ind];
					sigmaa[klabels[ind]] += m_avec[ind];
					sigmab[klabels[ind]] += m_bvec[ind];
//                  if (m_Gradient->Magnitude(ind) > 0)
//                  {
//                      sigmagx[klabels[ind]] += m_Gradient->GradientX(ind);
//                      sigmagy[klabels[ind]] += m_Gradient->GradientY(ind);
//                  }
					sigmax[klabels[ind]] += c;
					sigmay[klabels[ind]] += r;
					//------------------------------------
					//edgesum[klabels[ind]] += edgemag[ind];
					//------------------------------------
					clustersize[klabels[ind]] += 1.0;
					ind++;
				}
			}
		}
		{
			for (int k = 0; k < numk; k++)
			{
				if (clustersize[k] <= 0)
				{
					clustersize[k] = 1;
				}
				inv[k] = 1.0 / clustersize[k]; //computing inverse now to multiply, than divide later
			}
		}
		{
			for (int k = 0; k < numk; k++)
			{
				kseedsl[k] = sigmal[k] * inv[k];
				kseedsa[k] = sigmaa[k] * inv[k];
				kseedsb[k] = sigmab[k] * inv[k];
				kseedsgx[k] = sigmagx[k] * inv[k];
				kseedsgy[k] = sigmagy[k] * inv[k];
				kseedsx[k] = sigmax[k] * inv[k];
				kseedsy[k] = sigmay[k] * inv[k];
				//------------------------------------
				//edgesum[k] *= inv[k];
				//------------------------------------
			}
		}
	}
}

void MySLIC::FindNeighbors(
	SLICLabels& labels,
	int*& labels_map)
{
	#pragma omp parallel for
	for (int l = 0; l < labels.size(); l++)
	{
		std::vector<bool> is_neighbor;
		is_neighbor.resize(labels.size(), false);
		int vx[] = {1, 0, -1, 0};
		int vy[] = {0, 1, 0, -1};
		for (int i = 0; i < labels[l].pixels.size(); i++)
		{
			int label = labels_map[labels[l].pixels[i].x + labels[l].pixels[i].y * m_width];
			for (int j = 0; j < 4 ; j++)
			{
				int n_offset_x = labels[l].pixels[i].x + vx[j];
				int n_offset_y = labels[l].pixels[i].y + vy[j];
				if (n_offset_x >= 0 && n_offset_x < m_width &&
						n_offset_y >= 0 && n_offset_y < m_height)
				{
					int n_label = labels_map[n_offset_x + n_offset_y * m_width];
					if (label != n_label)
					{
						is_neighbor[n_label] = true;
						labels[l].n_boundary[n_label].push_back(labels[l].pixels[i]);
					}
				}
			}
		}
		for (int i = 0; i < is_neighbor.size(); i++)
		{
			if (is_neighbor[i])
			{
				labels[l].neighbors.push_back(i);
			}
		}
	}
}

void MySLIC::FindBoxBoundary(SLICLabels& labels)
{
	#pragma omp parallel for
	for (int l = 0; l < labels.size(); l++)
	{
		for (int i = 0; i < labels[l].pixels.size(); i++)
		{
			if (labels[l].pixels[i].x > labels[l].boundary.max_x)
			{
				labels[l].boundary.max_x = labels[l].pixels[i].x;
			}
			if (labels[l].pixels[i].x < labels[l].boundary.min_x)
			{
				labels[l].boundary.min_x = labels[l].pixels[i].x;
			}
			if (labels[l].pixels[i].y > labels[l].boundary.max_y)
			{
				labels[l].boundary.max_y = labels[l].pixels[i].y;
			}
			if (labels[l].pixels[i].y < labels[l].boundary.min_y)
			{
				labels[l].boundary.min_y = labels[l].pixels[i].y;
			}
		}
	}
}

void MySLIC::GetRGBtoLABConversion(
	const unsigned int* ubuff,
	LabColor*& labbuff)
{
	int sz = m_width * m_height;
	labbuff = new LabColor[sz];
	for (int j = 0; j < sz; j++)
	{
		int r = (ubuff[j] >> 16) & 0xFF;
		int g = (ubuff[j] >>  8) & 0xFF;
		int b = (ubuff[j]) & 0xFF;
		RGB2LAB(r, g, b, labbuff[j].l, labbuff[j].a, labbuff[j].b);
	}
}

double MySLIC::MaxDistanceofColor(
	SLICLabel& label)
{
	double max_d = 0;
	double min_d = 9999999;
	for (int i = 0; i < label.pixels.size(); i++)
	{
		double l = m_lvec[label.pixels[i].x + label.pixels[i].y * m_width];
		double a = m_avec[label.pixels[i].x + label.pixels[i].y * m_width];
		double b = m_bvec[label.pixels[i].x + label.pixels[i].y * m_width];
		double d = l * l + a * a + b * b;
		if (max_d < d)
		{
			max_d = d;
		}
		if (min_d > d)
		{
			min_d = d;
		}
	}
	return max_d - min_d;
}

double MySLIC::MaxDistanceofCenterAndSeed(SLICLabel& label)
{
	double cx = (label.boundary.max_x + label.boundary.min_x) * 0.5;
	double cy = (label.boundary.max_y + label.boundary.min_y) * 0.5;
	return (label.seed_xy.x - cx) * (label.seed_xy.x - cx) + (label.seed_xy.y - cy) *
		   (label.seed_xy.y - cy);
}

double MySLIC::VarLab(SLICLabel& label)
{
	double mu = AvgLab(label);
	double sum = 0;
	for (int i = 0; i < label.pixels.size(); i++)
	{
		int idx = label.pixels[i].x + label.pixels[i].y * m_width;
		double value = sqrt(m_lvec[idx] * m_lvec[idx] + m_avec[idx] * m_avec[idx] + m_bvec[idx] *
							m_bvec[idx]) - mu;
		sum += value * value;
	}
	return sum /= label.pixels.size();
}

double MySLIC::AvgLab(SLICLabel& label)
{
	double sum = 0;
	for (int i = 0; i < label.pixels.size(); i++)
	{
		int idx = label.pixels[i].x + label.pixels[i].y * m_width;
		sum += sqrt(m_lvec[idx] * m_lvec[idx] + m_avec[idx] * m_avec[idx] + m_bvec[idx] * m_bvec[idx]);
	}
	return sum /= label.pixels.size();
}

void MySLIC::ExtractLabel(
	SLICLabel& label,
	unsigned int* label_img)
{
	int label_width  = label.boundary.max_x - label.boundary.min_x;
	int label_height = label.boundary.max_y - label.boundary.min_y;
	label_img = new unsigned int[label_width * label_height];
	memset(label_img, 0, label_height * label_width * sizeof(unsigned int));
	for (int i = 0; i < label.pixels.size(); i++)
	{
		int idx = label.pixels[i].x + label.pixels[i].y * m_width;
		label_img[i] = m_Buff[idx];
	}
}

double MySLIC::GetDisLab(int x1, int y1, int x2, int y2)
{
	int idx1 = x1 + y1 * m_width;
	int idx2 = x2 + y2 * m_width;
	return sqrt((m_lvec[idx1] - m_lvec[idx2]) * (m_lvec[idx1] - m_lvec[idx2])
				+ (m_avec[idx1] - m_avec[idx2]) * (m_avec[idx1] - m_avec[idx2])
				+ (m_bvec[idx1] - m_bvec[idx2]) * (m_bvec[idx1] - m_bvec[idx2]));
}

double MySLIC::ColorError(const LabColor& c1, const LabColor& c2)
{
	double l = c1.l - c2.l;
	double a = c1.a - c2.a;
	double b = c1.b - c2.b;
	return sqrt(l * l + a * a + b * b);
}

void MySLIC::AddNeighbors(int idx, SLICLabelPatch& patch, const SLICLabels& labels, ints& marked,
						  double threshold)
{
	intds stacks;
	stacks.push_back(idx);
	for (; !stacks.empty();)
	{
		const ints& nebors = labels[stacks.front()].neighbors;
		for (int i = 0; i < nebors.size(); ++i)
		{
			int neid = nebors[i];
			if (marked.at(neid))
			{
				continue;
			}
			double error = ColorError(labels[neid].seed_lab, labels[idx].seed_lab);
			if (error < threshold)
			{
				patch.idxs.push_back(neid);
				marked[neid] = 1;
				stacks.push_back(neid);
			}
		}
		stacks.pop_front();
	}
}

void MySLIC::ComputePatchFromColor(const SLICLabels& labels, SLICLabelPatchs& patchs,
								   double threshold)
{
	ints marked(labels.size(), 0);
	for (int i = 0; i < labels.size(); ++i)
	{
		if (!marked[i])
		{
			SLICLabelPatch patch;
			patch.pid = patchs.size();
			patch.idxs.push_back(i);
			AddNeighbors(i, patch, labels, marked, threshold);
			patchs.push_back(patch);
		}
	}
}

ColorConstraints MySLIC::BuildColorConstraints(const SLICLabels& labels,
		const SLICLabelPatchs& patchs, cv::Mat img)
{
	cv::Mat nimg = img.clone();
	cv::resize(nimg, nimg, nimg.size()*2, 0, 0, cv::INTER_CUBIC);
	ColorConstraints res;
	for (int i = 0; i < patchs.size(); ++i)
	{
		res.push_back(ColorConstraint());
		ColorConstraint& ncc = res.back();
		const ints& idxs = patchs[i].idxs;
		for (int j = 0; j < idxs.size(); ++j)
		{
			const Pixels& pixels = labels[idxs[j]].pixels;
			for (int k = 0; k < pixels.size(); ++k)
			{
				ncc.AddPoint(pixels[k].x, pixels[k].y, img.at<cv::Vec3b>(pixels[k].y/2, pixels[k].x/2));
			}
		}
		for (int j = 0; j < idxs.size(); ++j)
		{
			const Pixels& pixels = labels[idxs[j]].pixels;
			for (int k = 0; k < pixels.size(); ++k)
			{
				nimg.at<cv::Vec3b>(pixels[k].y, pixels[k].x) = ncc.GetColorCvPoint(pixels[k].x, pixels[k].y);
			}
		}
	}
	cv::imshow("nimg", nimg);
	return res;
}



XYZColor LAB2XYZ(LabColor& val)
{
	double  var_Y = (val.l + 16) / 116;
	double  var_X = val.a / 500 + var_Y;
	double  var_Z = var_Y - val.b / 200;
	double kappa   = 903.3;     //actual CIE standard
	if (pow(var_X, 3) > 0.008856)
	{
		var_X = pow(var_X, 3);
	}
	else
	{
		var_X = ((var_X * 116) - 16) / kappa;
	}
	if (pow(var_Y, 3) > 0.008856)
	{
		var_Y = pow(var_Y, 3);
	}
	else
	{
		var_Y = ((var_Y * 116) - 16) / kappa;
	}
	if (pow(var_Z, 3) > 0.008856)
	{
		var_Z = pow(var_Z, 3);
	}
	else
	{
		var_Z = ((var_Z * 116) - 16) / kappa;
	}
	double Xr = 0.950456;   //reference white
	double Yr = 1.0;        //reference white
	double Zr = 1.088754;   //reference white
	XYZColor result;
	result.x = Xr * var_X;
	result.y = Yr * var_Y;
	result.z = Zr * var_Z;
	return result;
}

RGBColor LAB2RGB(LabColor& val)
{
	XYZColor var = LAB2XYZ(val);
	double  var_B = var.x *  3.2406 + var.y * -1.5372 + var.z * -0.4986;
	double  var_G = var.x * -0.9689 + var.y *  1.8758 + var.z *  0.0415;
	double  var_R = var.x *  0.0557 + var.y * -0.2040 + var.z *  1.0570;
	if (var_R > 0.0031308)
	{
		var_R = 1.055 * pow(var_R, 1.0 / 2.4) - 0.055;
	}
	else
	{
		var_R = 12.92 * var_R;
	}
	if (var_G > 0.0031308)
	{
		var_G = 1.055 * pow(var_G, 1.0 / 2.4) - 0.055;
	}
	else
	{
		var_G = 12.92 * var_G;
	}
	if (var_B > 0.0031308)
	{
		var_B = 1.055 * pow(var_B, 1.0 / 2.4) - 0.055;
	}
	else
	{
		var_B = 12.92 * var_B;
	}
	RGBColor result;
	result.r = (int)(var_R * 255);
	result.g = (int)(var_G * 255);
	result.b = (int)(var_B * 255);
	return result;
}

XYZColor RGB2XYZ(RGBColor& val)
{
	double R = (double)val.r / 255.0;
	double G = (double)val.g / 255.0;
	double B = (double)val.b / 255.0;
	double r, g, b;
	if (R <= 0.04045)
	{
		r = R / 12.92;
	}
	else
	{
		r = pow((R + 0.055) / 1.055, 2.4);
	}
	if (G <= 0.04045)
	{
		g = G / 12.92;
	}
	else
	{
		g = pow((G + 0.055) / 1.055, 2.4);
	}
	if (B <= 0.04045)
	{
		b = B / 12.92;
	}
	else
	{
		b = pow((B + 0.055) / 1.055, 2.4);
	}
	XYZColor result;
	result.x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
	result.y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
	result.z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;
	return result;
}

LabColor RGB2LAB(RGBColor& val)
{
	//------------------------
	// sRGB to XYZ conversion
	//------------------------
	XYZColor var;
	var = RGB2XYZ(val);
	//------------------------
	// XYZ to LAB conversion
	//------------------------
	double epsilon = 0.008856;  //actual CIE standard
	double kappa   = 903.3;     //actual CIE standard
	double Xr = 0.950456;   //reference white
	double Yr = 1.0;        //reference white
	double Zr = 1.088754;   //reference white
	double xr = var.x / Xr;
	double yr = var.y / Yr;
	double zr = var.z / Zr;
	double fx, fy, fz;
	if (xr > epsilon)
	{
		fx = pow(xr, 1.0 / 3.0);
	}
	else
	{
		fx = (kappa * xr + 16.0) / 116.0;
	}
	if (yr > epsilon)
	{
		fy = pow(yr, 1.0 / 3.0);
	}
	else
	{
		fy = (kappa * yr + 16.0) / 116.0;
	}
	if (zr > epsilon)
	{
		fz = pow(zr, 1.0 / 3.0);
	}
	else
	{
		fz = (kappa * zr + 16.0) / 116.0;
	}
	LabColor result;
	result.l = 116.0 * fy - 16.0;
	result.a = 500.0 * (fx - fy);
	result.b = 200.0 * (fy - fz);
	return result;
}

