#ifndef __POINTMAP_HEADER__
#define __POINTMAP_HEADER__

#define _USE_MATH_DEFINES

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct Point{
	float pos[3];
	short plane;
	double normal[3];
	unsigned char color[3];
}Point;

typedef struct NearestPoints{
	int max;
	int found;
	int got_heap;
	float pos[3];
	float *dist2;
	const Point **index;
}NearestPoints;

class PointMap{
public:
	PointMap(const int max_points);
	~PointMap();
	void store(const float pos[3], const float normal[3], unsigned char color[3]);
	void balance(void);
	void locate_points(NearestPoints *const np, const int index) const;

	Point* temp;
	void set_temp();
private:
	Point *points;

	int stored_points;
	int max_points;

	float bbox_min[3];
	float bbox_max[3];

	void balance_segment(Point **pbal, Point **porg, const int index, const int start, const int end);
	void median_split(Point **p, const int start, const int end, const int median, const int axis);
};

#endif