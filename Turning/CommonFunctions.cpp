/* -*-c++-*- OpenSceneGraph Cookbook
 * Common functions
 * Author: Wang Rui <wangray84 at gmail dot com>
*/
#include "StdAfx.h"
#include <osg/PolygonMode>
#include <osgText/Font>
#include <osgViewer/View>
#include <cmath>

#include "CommonFunctions.h"

namespace osgCookBook
{
    osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/arial.ttf");

    osg::AnimationPathCallback* createAnimationPathCallback( float radius, float time )
    {
        osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
        path->setLoopMode( osg::AnimationPath::LOOP );

        unsigned int numSamples = 32;
        float delta_yaw = 2.0f * osg::PI/((float)numSamples - 1.0f);
        float delta_time = time / (float)numSamples;
        for ( unsigned int i=0; i<numSamples; ++i )
        {
            float yaw = delta_yaw * (float)i;
            osg::Vec3f pos( sin(yaw)*radius, cos(yaw)*radius, 0.0f );
            osg::Quat rot( -yaw, osg::Z_AXIS );
            path->insert( delta_time * (float)i, osg::AnimationPath::ControlPoint(pos, rot) );
        }

        osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
        apcb->setAnimationPath( path.get() );
        return apcb.release();
    }

    osg::Camera* createRTTCamera( osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute )
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setClearColor( osg::Vec4() );
        camera->setClearMask( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        camera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
        camera->setRenderOrder( osg::Camera::PRE_RENDER );
        if ( tex )
        {
            tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
            tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
            camera->setViewport( 0, 0, tex->getTextureWidth(), tex->getTextureHeight() );
            camera->attach( buffer, tex );
        }

        if ( isAbsolute )
        {
            camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
            camera->setProjectionMatrix( osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0) );
            camera->setViewMatrix( osg::Matrix::identity() );
            camera->addChild( createScreenQuad(1.0f, 1.0f) );
        }
        return camera.release();
    }

    osg::Camera* createHUDCamera( float left, float right, float bottom, float top )
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        camera->setClearMask( GL_DEPTH_BUFFER_BIT );
        camera->setRenderOrder( osg::Camera::POST_RENDER );
        camera->setAllowEventFocus( false );
        camera->setProjectionMatrix( osg::Matrix::ortho2D(left, right, bottom, top) );
        camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
        return camera.release();
    }

    osg::Geode* createScreenQuad( float width, float height, float scale )
    {
        osg::Geometry* geom = osg::createTexturedQuadGeometry(
            osg::Vec3f(), osg::Vec3f(width,0.0f,0.0f), osg::Vec3f(0.0f,height,0.0f),
            0.0f, 0.0f, width*scale, height*scale );
        osg::ref_ptr<osg::Geode> quad = new osg::Geode;
        quad->addDrawable( geom );

        int values = osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED;
        quad->getOrCreateStateSet()->setAttribute(
            new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL), values );
        quad->getOrCreateStateSet()->setMode( GL_LIGHTING, values );
        return quad.release();
    }


	// it is better that the image shape is propotional to the quad
	osg::ref_ptr<osg::Geode> createQuadwithTexture(int imageRepeatNum, float quadLength, float quadWidth, osg::ref_ptr<osg::Image>image)
	{
		float x = quadLength*0.5f, y = quadWidth * 0.5f;
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back( osg::Vec3(-x, -y, 0.0f) );
		vertices->push_back( osg::Vec3(x, -y, 0.0f) );
		vertices->push_back( osg::Vec3(x, y, 0.0f) );
		vertices->push_back( osg::Vec3(-x, y, 0.0f) );

		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		normals->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );

		osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
		texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
		texcoords->push_back( osg::Vec2(float(imageRepeatNum), 0.0f) );
		texcoords->push_back( osg::Vec2(float(imageRepeatNum), float(imageRepeatNum)) );
		texcoords->push_back( osg::Vec2(0.0f, float(imageRepeatNum)) );

		osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
		quad->setVertexArray(vertices.get());
		quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
		quad->setTexCoordArray( 0, texcoords.get() );
		quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );

		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage( image.get() );
		if(imageRepeatNum > 1)
		{
			texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
			texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		}

		osg::ref_ptr<osg::Geode> quadGeode = new osg::Geode;
		quadGeode->addDrawable( quad.get() );
		quadGeode->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );

		return quadGeode.get();
	}



    osgText::Text* createText( const osg::Vec3f& pos, const std::string& content, float size )
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setDataVariance( osg::Object::DYNAMIC );
        text->setFont( g_font.get() );
        text->setCharacterSize( size );
        text->setAxisAlignment( osgText::TextBase::XY_PLANE );
        text->setPosition( pos );
        text->setText( content );
        return text.release();
    }

    float randomValue( float min, float max )
    {
        return (min + (float)rand()/(RAND_MAX+1.0f) * (max - min));
    }

    osg::Vec3f randomVector( float min, float max )
    {
        return osg::Vec3f( randomValue(min, max),
                          randomValue(min, max),
                          randomValue(min, max) );
    }

    osg::Matrix randomMatrix( float min, float max )
    {
        osg::Vec3f rot = randomVector(-osg::PI, osg::PI);
        osg::Vec3f pos = randomVector(min, max);
        return osg::Matrix::rotate(rot[0], osg::X_AXIS, rot[1], osg::Y_AXIS, rot[2], osg::Z_AXIS) *
               osg::Matrix::translate(pos);
    }


	// will add a MatrixTransform to pNode (does not handle multiple parents!)
	osg::MatrixTransform* AddMatrixTransform( osg::Node* pNode )
	{
		// parent must derive from osg::Group
		osg::Group* pGroup = pNode->getParent(0)->asGroup();
		if ( pGroup )
		{
			// make sure we have a reference count at all time!
			osg::ref_ptr<osg::Node> pNodeTmp = pNode;

			// remove pNode from parent
			pGroup->removeChild( pNodeTmp );

			// create MatrixTransform and do connections
			osg::MatrixTransform* pMT = new osg::MatrixTransform;
			pMT->addChild( pNodeTmp );
			pGroup->addChild( pMT );
			return pMT;
		}

		return 0;
	}


	osg::ref_ptr<osg::StateSet> setFogState(osg::Vec4& fogcolor, float fogdensity, float fogstart, float fogend)
	{
		osg::ref_ptr<osg::Fog> fog = new osg::Fog();
		fog->setMode(osg::Fog::LINEAR);
		fog->setColor(fogcolor);
		fog->setDensity(fogdensity);
		
		fog->setStart(fogstart);
		fog->setEnd(fogend);

		osg::ref_ptr< osg::StateSet> fogState (new osg::StateSet);
		fogState->setAttributeAndModes(fog.get(),osg::StateAttribute::ON);
		fogState->setMode(GL_FOG,osg::StateAttribute::ON);

		return fogState.get();
	}


	osg::ref_ptr<osgParticle::PrecipitationEffect> initPrecipitationEffect()
	{
		osg::ref_ptr<osgParticle::PrecipitationEffect> precipitationEffect = new osgParticle::PrecipitationEffect;
		precipitationEffect->rain(0.0);
		precipitationEffect->snow(0.0);
		//precipitationEffect->setCellSize(osg::Vec3(200.0f, 200.0f, 200.0f));
		//precipitationEffect->setParticleSize(0.8f);
		precipitationEffect->setParticleColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//precipitationEffect->setWind(osg::Vec3(2, 0, 0));
		//precipitationEffect->setNearTransition(0.5f);
		//precipitationEffect->setFarTransition(10.0f);
		precipitationEffect->setUseFarLineSegments(true);

		return precipitationEffect.get();
	}


	osg::Node* FindNodeByName( osg::Node* pNode, const std::string& sName )
	{
		if ( pNode->getName()==sName )
		{
			return pNode;
		}

		osg::Group* pGroup = pNode->asGroup();
		if ( pGroup )
		{
			for ( unsigned int i=0; i<pGroup->getNumChildren(); i++ )
			{
				osg::Node* pFound = FindNodeByName( pGroup->getChild(i), sName );
				if ( pFound )
				{
					return pFound;
				}
			}
		}

		return 0;
	}


	bool PickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		if ( ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE ||
			ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ||
			!(ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) )
			return false;

		osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
		if ( viewer )
		{
			osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
				new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
			osgUtil::IntersectionVisitor iv( intersector.get() );
			viewer->getCamera()->accept( iv );

			if ( intersector->containsIntersections() )
			{
				osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());
				doUserOperations( result );
			}
		}
		return false;
	}

	GeodeFinder::GeodeFinder ()
		: NodeVisitor (osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

	void GeodeFinder::apply (osg::Node &searchNode) {
		if (! strcmp (searchNode.className(), "Geode")) {
			foundGeodes.push_back ((osg::Geode*) &searchNode);
		}

		traverse (searchNode);
	}

	osg::Geode* GeodeFinder::getFirst () {
		if (foundGeodes.size() > 0)
			return foundGeodes.at(0);
		else
			return NULL;
	}

	std::vector<osg::Geode*> GeodeFinder::getNodeList() {
		return foundGeodes;
	}

	// Given a valid node placed in a scene under a transform, return the
	// world coordinates in an osg::Matrix.
	// Creates a visitor that will update a matrix representing world coordinates
	// of the node, return this matrix.
	// (This could be a class member for something derived from node also.

	osg::Matrix* getWorldCoords( osg::Node* node)
	{
		getWorldCoordOfNodeVisitor* ncv = new getWorldCoordOfNodeVisitor();
		if (node && ncv)
		{
			node->accept(*ncv);
			return ncv->giveUpDaMat();
		}
		else
		{
			return NULL;
		}
	}
}