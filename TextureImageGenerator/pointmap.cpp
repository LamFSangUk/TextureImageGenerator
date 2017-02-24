#ifndef __POINTMAP__
#define __POINTMAP__

#include"pointmap.h"

PointMap::PointMap(const int max_points){

	this->stored_points = 0;
	this->max_points = max_points;

	this->points = (Point*)malloc(sizeof(Point)*(max_points + 1));

	bbox_min[0] = bbox_min[1] = bbox_min[2] = 1e8f;
	bbox_max[0] = bbox_max[1] = bbox_max[2] = -1e8f;

}

PointMap::~PointMap(){
	free(points);
}

void PointMap::store(const float pos[3], const float normal[3], unsigned char color[3]){

	if (stored_points >= max_points)
		return;

	stored_points++;
	Point *const node = &points[stored_points];

	for (int i = 0; i < 3; i++){
		node->pos[i] = pos[i];

		if (node->pos[i]<bbox_min[i])
			bbox_min[i] = node->pos[i];
		if (node->pos[i]>bbox_max[i])
			bbox_max[i] = node->pos[i];

		node->color[i] = color[i];
	}

	int theta = int(acos(normal[2])*(256.0 / M_PI));
	if (theta > 255)
		node->theta = 255;
	else
		node->theta = (unsigned char)theta;

	int phi = int(atan2(normal[1], normal[0])*(256.0 / (2.0*M_PI)));
	if (phi > 255)
		node->phi = 255;
	else if (phi < 0)
		node->phi = (unsigned char)(phi + 256);
	else
		node->phi = (unsigned char)phi;
}

void PointMap::balance(void)
{
	if (stored_points > 1){
		//allocate two temporary arrays for the balancing procedure
		Point **pa1 = (Point**)malloc(sizeof(Point*)*(stored_points + 1));
		Point **pa2 = (Point**)malloc(sizeof(Point*)*(stored_points + 1));

		for (int i = 0; i <= stored_points; i++)
			pa2[i] = &points[i];

		balance_segment(pa1, pa2, 1, 1, stored_points);
		free(pa2);

		//reorganize balanced kd-tree (make a heap)
		int d, j = 1, foo = 1;
		Point foo_point = points[j];

		for (int i = 1; i <= stored_points; i++){
			d = pa1[j] - points;
			pa1[j] = NULL;
			if (d != foo)
				points[j] = points[d];
			else{
				points[j] = foo_point;

				if (i < stored_points){
					for (; foo <= stored_points; foo++)
						if (pa1[foo] != NULL)
							break;
					foo_point = points[foo];
					j = foo;
				}
				continue;
			}
			j = d;
		}
		free(pa1);
	}

	//half_stored_photons = stored_photons / 2 - 1;
}

void PointMap::balance_segment(Point **pbal, Point **porg, const int index, const int start, const int end){
	//compute new median

	int median = 1;
	while ((4 * median) <= (end - start + 1))
		median += median;

	if ((3 * median) <= (end - start + 1)){
		median += median;
		median += start - 1;
	}
	else
		median = end - median + 1;


	// find axis to split along

	int axis = 2;
	if ((bbox_max[0] - bbox_min[0]) > (bbox_max[1] - bbox_min[1]) &&
		(bbox_max[0] - bbox_min[0]) > (bbox_max[2] - bbox_min[2]))
		axis = 0;
	else if ((bbox_max[1] - bbox_min[1]) > (bbox_max[2] - bbox_min[2]))
		axis = 1;

	//partition photon block around the median

	median_split(porg, start, end, median, axis);

	pbal[index] = porg[median];
	pbal[index]->plane = axis;

	//recursively balance the left and right block

	if (median > start){
		//balance left segment
		if (start < median - 1){
			const float tmp = bbox_max[axis];
			bbox_max[axis] = pbal[index]->pos[axis];
			balance_segment(pbal, porg, 2 * index, start, median - 1);
			bbox_max[axis] = tmp;
		}
		else {
			pbal[2 * index] = porg[start];
		}
	}

	if (median < end){
		//balance right segment
		if (median + 1 < end){
			const float tmp = bbox_min[axis];
			bbox_min[axis] = pbal[index]->pos[axis];
			balance_segment(pbal, porg, 2 * index + 1, median + 1, end);
			bbox_min[axis] = tmp;
		}
		else{
			pbal[2 * index + 1] = porg[end];
		}
	}
}

#define swap(ph,a,b){Point *ph2=ph[a]; ph[a]=ph[b]; ph[b]=ph2;}

void PointMap::median_split(Point **p, const int start, const int end, const int median, const int axis){
	int left = start;
	int right = end;

	while (right > left){
		const float v = p[right]->pos[axis];
		int i = left - 1;
		int j = right;
		for (;;){
			while (p[++i]->pos[axis] < v)
				;
			while (p[--j]->pos[axis] > v&&j > left)
				;
			if (i >= j)
				break;
			swap(p, i, j);
		}

		swap(p, i, right);
		if (i >= median)
			right = i - 1;
		if (i <= median)
			left = i + 1;
	}
}

void PointMap::locate_points(
	NearestPoints *const np,
	const int index) const
{
	const Point *p = &points[index];
	float dist1;
	
	if (index <= stored_points/2){
		dist1 = np->pos[p->plane] - p->pos[p->plane];

		if (dist1 > 0.0) {// if dist1 is positive search right plane
			locate_points(np, 2 * index + 1);
			if (dist1*dist1 < np->dist2[0])
				locate_points(np, 2 * index);
		}
		else {// dist1 is negative search left first
			locate_points(np, 2 * index);
			if (dist1*dist1 < np->dist2[0])
				locate_points(np, 2 * index + 1);
		}
	}

	// compute squared distance between current photon and np->pos

	dist1 = p->pos[0] - np->pos[0];
	float dist2 = dist1*dist1;
	dist1 = p->pos[1] - np->pos[1];
	dist2 += dist1*dist1;
	dist1 = p->pos[2] - np->pos[2];
	dist2 += dist1*dist1;

	if (dist2 < np->dist2[0]){
		//we found a photon :) Insert it in the candidate list

		if (np->found < np->max){
			//heap is not full; use array
			np->found++;
			np->dist2[np->found] = dist2;
			np->index[np->found] = p;
		}
		else{
			int j, parent;

			if (np->got_heap == 0){ // Do we need to build the heap?
				//Build heap
				float dst2;
				const Point *phot;
				int half_found = np->found >> 1;
				for (int k = half_found; k >= 1; k--){
					parent = k;
					phot = np->index[k];
					dst2 = np->dist2[k];
					while (parent <= half_found){
						j = parent + parent;
						if (j<np->found && np->dist2[j] < np->dist2[j + 1])
							j++;
						if (dst2 >= np->dist2[j])
							break;
						np->dist2[parent] = np->dist2[j];
						np->index[parent] = np->index[j];
						parent = j;
					}
					np->dist2[parent] = dst2;
					np->index[parent] = phot;
				}
				np->got_heap = 1;
			}

			// Insert new photon into max heap
			// delete largest element, insert new, and reorder the heap

			parent = 1;
			j = 2;
			while (j <= np->found){
				if (j < np->found&&np->dist2[j] < np->dist2[j + 1])
					j++;
				if (dist2 > np->dist2[j])
					break;
				np->dist2[parent] = np->dist2[j];
				np->index[parent] = np->index[j];
				parent = j;
				j += j;
			}
			if (dist2 < np->dist2[parent]){
				np->index[parent] = p;
				np->dist2[parent] = dist2;
			}
			np->dist2[0] = np->dist2[1];
		}
	}
}

void PointMap::set_temp(){
	temp = points;
}

#endif