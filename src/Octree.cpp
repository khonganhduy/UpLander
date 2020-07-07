//  Kevin M. Smith - Basic Octree Class - CS134/235 4/18/18
//


#include "Octree.h"
 

// draw Octree (recursively)
//
void Octree::draw(TreeNode & node, int numLevels, int level) {
	ofNoFill();
	if (level >= numLevels) return;
	drawBox(node.box);
	
	switch (level)
	{
	case 0:
		ofSetColor(ofColor::crimson);
		break;
	case 1:
		ofSetColor(ofColor::yellow);
		break;
	case 2:
		ofSetColor(ofColor::darkOliveGreen);
		break;
	case 3:
		ofSetColor(ofColor::brown);
		break;
	case 4:
		ofSetColor(ofColor::blue);
		break;
	case 5:
		ofSetColor(ofColor::white);
		break;
	default:
		ofSetColor(ofColor::purple);
		break;
	}
	level++;
	for (int i = 0; i < node.children.size(); i++) {
		draw(node.children[i], numLevels, level);
	}
}

// draw only leaf Nodes
//
void Octree::drawLeafNodes(TreeNode & node) {
	ofNoFill();
	ofSetColor(ofColor::orange);
	if (!node.points.empty())
	{
		drawBox(node.box);
	}
	else
	{
		for (int i = 0; i < node.children.size(); i++)
		{
			drawLeafNodes(node.children[i]);
		}
	}
}


//draw a box from a "Box" class  
//
void Octree::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box Octree::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	cout << "vertices: " << n << endl;
//	cout << "min: " << min << "max: " << max << endl;
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//mesh here is the mesh of the map terrain
//vector points is the collection of all the points inside the mesh
//vector pointsRtn should be an empty vector at the beginnning; should have all points of the mesh
//that are inside the box
int Octree::getMeshPointsInBox(const ofMesh & mesh, const vector<int>& points,
	Box & box, vector<int> & pointsRtn)
{
	int count = 0;
	for (int i = 0; i < points.size(); i++) {
		ofVec3f v = mesh.getVertex(points[i]);
		if (box.inside(Vector3(v.x, v.y, v.z))) {
			count++;
			pointsRtn.push_back(points[i]);
		}
	}
	return count;
}


//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void Octree::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

// CS 134 - Spring 2019
// Copyright (c) Kevin Smith March 2019
//
//

//  primary recursive function to build octree.
//  algorithm:
//  1) return when the level is numLevels or greater
//  2) subdivide the current node box into 8 boxes
//  3) for each child box:
//      (a) distribute points in parent node to child node
//      (b) if there are points in child node add it to parent's children
//      (c) if a child has more than one point
//            recursively call subdivide on child
//
/*
void Octree::subdivide(TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	vector<Box> boxList;
	subDivideBox8(node.box, boxList);
	level++;
	for (int i = 0; i < boxList.size(); i++) {
		TreeNode child;
		int count = getMeshPointsInBox(mesh, node.points, boxList[i], child.points);
		if (count > 0) {
			child.box = boxList[i];
			node.children.push_back(child);
			if (count > 1) {
				subdivide(node.children[node.children.size() - 1], numLevels, level);
			}
		}
	}
}*/


//Harry Vo-create method
void Octree::create(const ofMesh & geo, int numLevels) {
	// initialize octree structure
	//
	mesh = geo;
	root.box = this->meshBounds(mesh);
	for (int i = 0; i < mesh.getNumVertices(); i++)
	{
		root.points.push_back(i);
	}
	subdivide(root, numLevels, 1);
	//subdivide(mesh, root, numLevels, 1); //linked to Harry Vo subdivide
}

void Octree::subdivide(TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
		vector<Box> childBoxes;
		subDivideBox8(node.box, childBoxes);
		vector<int> points = node.points;
		node.points.clear();
		for (int i = 0; i < childBoxes.size(); i++) {
			TreeNode n;
			if (getMeshPointsInBox(mesh, points, childBoxes[i], n.points) > 0) {
				n.box = childBoxes[i];
				
			if (getMeshPointsInBox(mesh, points, n.box, n.points) > 1)
			{
				subdivide(n, numLevels, level + 1);
			}
			node.children.push_back(n);
			}
		node.points.clear();
	}
}

bool Octree::intersect(const Ray &ray, const TreeNode & node, TreeNode & nodeRtn, glm::vec3 camOrigin) {
	if (node.box.intersect(ray, 0, 10000))
	{
		if (node.points.empty())
		{
			for (int i = 0; i < node.children.size(); i++)
			{
				this->intersect(ray, node.children[i], nodeRtn, camOrigin);
			}
		}
		else
		{
			intersecting.push_back(node);
			nodeRtn = closest(camOrigin);
		}
		return true;
	}
	return false;
}

TreeNode Octree::closest(glm::vec3 camOrigin)
{
	if (intersecting.size() == 1)
	{
		return intersecting[0];
	}
	else
	{
		TreeNode closest;
		BOOLEAN firstTime = true;
		int shortestDist = 0;
		for (int i = 0; i < intersecting.size(); i++)
		{
			glm::vec3 boxCenter = glm::vec3(intersecting[i].box.center().x(), intersecting[i].box.center().y(), intersecting[i].box.center().z());
			glm::vec3 dist = boxCenter - camOrigin;
			if (firstTime || shortestDist < abs(dist.length()))
			{
				if (firstTime)
				{
					firstTime = false;
				}
				closest = intersecting[i];
				shortestDist = abs(dist.length());
			}
		}
		intersecting.clear();
		return closest;
	}
}



