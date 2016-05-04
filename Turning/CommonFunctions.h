#pragma once
/* -*-c++-*- OpenSceneGraph Cookbook
 * Common functions
 * Author: Wang Rui <wangray84 at gmail dot com>
*/

#ifndef H_COOKBOOK_COMMONFUNCTIONS
#define H_COOKBOOK_COMMONFUNCTIONS

#include <osg/AnimationPath>
#include <osg/Texture>
#include <osg/Camera>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgText/Text>
#include <osgUtil/LineSegmentIntersector>
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Node>
#include <osg/Geode>
#include <osg/ComputeBoundsVisitor>
#include <osg/Fog>
#include <osg/TextureCubeMap>
#include <osg/TexGen>
#include <osg/TexMat>
#include <osg/Depth>
#include <osg/ShapeDrawable>
#include <osgParticle/PrecipitationEffect>

using namespace std;

namespace osgCookBook
{
	extern osg::AnimationPathCallback* createAnimationPathCallback( float radius, float time );
    extern osg::Camera* createRTTCamera( osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute=false );
    extern osg::Camera* createHUDCamera( float left, float right, float bottom, float top );
    extern osg::Geode* createScreenQuad( float width, float height, float scale=1.0f );
    extern osgText::Text* createText( const osg::Vec3f& pos, const std::string& content, float size );

	extern osg::ref_ptr<osg::Geode> createQuadwithTexture(int imageRepeatNum, float quadLength, float quadWidth, osg::ref_ptr<osg::Image>image);

    extern float randomValue( float min, float max );
    extern osg::Vec3f randomVector( float min, float max );
    extern osg::Matrix randomMatrix( float min, float max );

	extern osg::Matrixd* getWorldCoords( osg::Node* node);

	extern osg::ref_ptr<osg::StateSet> setFogState(osg::Vec4& fogcolor, float fogdensity, float fogstart, float fogend);

	extern osg::ref_ptr<osgParticle::PrecipitationEffect> initPrecipitationEffect();
	// these two functions are from this link: osghelp.com/?p=72
	extern osg::MatrixTransform* AddMatrixTransform( osg::Node* pNode );
	extern osg::Node* FindNodeByName( osg::Node* pNode, const std::string& sName );

	class PickHandler : public osgGA::GUIEventHandler
	{
	public:
		virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
		virtual void doUserOperations( osgUtil::LineSegmentIntersector::Intersection& result ) = 0;
	};

	// Visitor to return the world coordinates of a node.
	// It traverses from the starting node to the parent.
	// The first time it reaches a root node, it stores the world coordinates of
	// the node it started from.  The world coordinates are found by concatenating all
	// the matrix transforms found on the path from the start node to the root node.

	class getWorldCoordOfNodeVisitor : public osg::NodeVisitor
	{
	public:
		getWorldCoordOfNodeVisitor():
		  osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), done(false)
		  {
			  wcMatrix= new osg::Matrixd();
		  }
		  virtual void apply(osg::Node &node)
		  {
			  if (!done)
			  {
				  if ( 0 == node.getNumParents() ) // no parents
				  {
					  wcMatrix->set( osg::computeLocalToWorld(this->getNodePath()) );
					  done = true;
				  }
				  traverse(node);
			  }
		  }
		  osg::Matrixd* giveUpDaMat()
		  {
			  return wcMatrix;
		  }
	private:
		bool done;
		osg::Matrix* wcMatrix;
	};

	class GeodeFinder : public osg::NodeVisitor {
	public:

		// Constructor - sets the traversal mode to TRAVERSE_ALL_CHILDREN
		// and Visitor type to NODE_VISITOR
		GeodeFinder();

		// The 'apply' method for 'node' type instances.
		// See if a className() call to searchNode returns "Geode."
		// If so, add this node to our list.
		void apply(osg::Node &searchNode);

		// Return a pointer to the first node in the list
		// with a matching name
		osg::Geode* getFirst();

		// return a the list of nodes we found
		std::vector<osg::Geode*> getNodeList();

	private:
		// List of nodes with names that match the searchForName string
		std::vector<osg::Geode*> foundGeodes;
	};
}

#endif