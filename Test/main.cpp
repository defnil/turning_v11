#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Light>
#include <osg/TexMat>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <iostream>

using namespace osg;
using namespace osgGA;

#define TEST_TANK 1
#ifdef TEST_TANK
StateSet *sceneStateSet;

// key controls
bool turnLeft, turnRight, moveForward, moveBackward;

//TexMat *textureTransform;
MatrixTransform* mt;
PositionAttitudeTransform *lightTransform;

Matrix matrix;
Vec3 pos = Vec3(0,0,1);
Vec2 direction = Vec2(0,1);
float rot=osg::PI_2;
float breakfact=1;
float speed=0;

class KeyboardHandler: public GUIEventHandler {
public:
	virtual bool handle(
		const GUIEventAdapter& ea, GUIActionAdapter& aa
		) {
			bool newState;
			switch (ea.getEventType()) {
			case GUIEventAdapter::KEYDOWN:
				newState = true;
				std::cout<<"newState down = "<<newState<<std::endl;
				break;
			case GUIEventAdapter::KEYUP:
				newState = false;
				std::cout<<"newState up = "<<newState<<std::endl;
				break;
			default: break;
				//return false;
			}
			switch (ea.getKey()) {
			case GUIEventAdapter::KEY_Left:
				turnLeft = newState;
				std::cout<<"left  = "<<turnLeft<<std::endl<<std::endl;
				break;
			case GUIEventAdapter::KEY_Right:
				turnRight = newState;
				std::cout<<"right  = "<<turnRight<<std::endl<<std::endl;
				break;

			case GUIEventAdapter::KEY_Down:
				moveBackward = newState;
				std::cout<<"back  = "<<moveBackward<<std::endl<<std::endl;
				break;
			default: break;
				//return false;
			}

			switch (ea.getKey())
			{
			case GUIEventAdapter::KEY_Up:
				moveForward = newState;
				std::cout<<"forward  = "<<moveForward<<std::endl<<std::endl;
				break;
			default: break;
			}

			return false;
	}
	virtual void accept(GUIEventHandlerVisitor& v) {
		v.visit(*this);
	};
};

Geode* createPlane(int size, int texSize) {
	// vertex array
	Vec3Array *vertexArray = new Vec3Array();
	vertexArray->push_back(Vec3(-size/2,-size/2,0));
	vertexArray->push_back(Vec3(-size/2,size/2,0));
	vertexArray->push_back(Vec3(size/2,size/2,0));
	vertexArray->push_back(Vec3(size/2,-size/2,0));

	// face array
	// give indices of vertices in counter-clockwise order
	DrawElementsUInt *faceArray = new DrawElementsUInt(
		PrimitiveSet::QUADS, 0
		);
	faceArray->push_back(0);
	faceArray->push_back(1);
	faceArray->push_back(2);
	faceArray->push_back(3);

	// normal array
	// give the normals on the cube's faces
	Vec3Array *normalArray = new Vec3Array();
	normalArray->push_back(Vec3(0, 0, +1));

	// normal index
	// assign each vertex a normal (similar to assigning colors)
	TemplateIndexArray<unsigned int, Array::UIntArrayType, 24, 4> *normalIndexArray;
	normalIndexArray = new TemplateIndexArray<
		unsigned int, Array::UIntArrayType, 24, 4
	>();
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);

	// texture coordinates
	osg::Vec2Array *texCoords = new osg::Vec2Array();
	texCoords->push_back(Vec2(0.0, 1.0));
	texCoords->push_back(Vec2(0.0, 0.0));
	texCoords->push_back(Vec2(1.0, 0.0));
	texCoords->push_back(Vec2(1.0, 1.0));

	Geometry *geometry = new Geometry();
	geometry->setVertexArray(vertexArray);
	geometry->addPrimitiveSet(faceArray);
	geometry->setNormalArray(normalArray);
	geometry->setNormalIndices(normalIndexArray);
	geometry->setNormalBinding(Geometry::BIND_PER_VERTEX);
	geometry->setTexCoordArray(0, texCoords);

	Geode *face = new Geode();
	face->addDrawable(geometry);

	return face;
}

Node *createTrack(double size, float texSize) {
	// create a plane
	Geode *geode;
	geode = createPlane(size, texSize);

	StateSet *stateSet = geode->getOrCreateStateSet();
	//stateSet->ref();

	Program *programObject = new Program();
	Shader *vertexObject = new Shader(Shader::VERTEX);
	Shader *fragmentObject = new Shader(Shader::FRAGMENT);
	programObject->addShader(fragmentObject);
	programObject->addShader(vertexObject);

	vertexObject->loadShaderSourceFromFile("track.vert");
	fragmentObject->loadShaderSourceFromFile("track.frag");

	stateSet->setAttributeAndModes(programObject, StateAttribute::ON);

	Image *tex1 = osgDB::readImageFile("tex1.jpg");
	if (!tex1) {
		std::cout << "Couldn't load texture tex1.jpg" << std::endl;
		return NULL;
	}

	Image *tex2 = osgDB::readImageFile("tex2.jpg");
	if (!tex1) {
		std::cout << "Couldn't load texture tex2.jpg" << std::endl;
		return NULL;
	}

	Image *mask = osgDB::readImageFile("mask.jpg");
	if (!mask) {
		std::cout << "Couldn't load texture mask.jpg" << std::endl;
		return NULL;
	}

	Texture2D *texture_tex1 = new Texture2D;
	texture_tex1->setDataVariance(Object::DYNAMIC);
	texture_tex1->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture_tex1->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	texture_tex1->setWrap(Texture::WRAP_S, Texture::REPEAT);
	texture_tex1->setWrap(Texture::WRAP_T, Texture::REPEAT);
	texture_tex1->setImage(tex1);

	Texture2D *texture_tex2 = new Texture2D;
	texture_tex2->setDataVariance(Object::DYNAMIC);
	texture_tex2->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture_tex2->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	texture_tex2->setWrap(Texture::WRAP_S, Texture::REPEAT);
	texture_tex2->setWrap(Texture::WRAP_T, Texture::REPEAT);
	texture_tex2->setImage(tex2);

	Texture2D *texture_mask = new Texture2D;
	texture_mask->setDataVariance(Object::DYNAMIC);
	texture_mask->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture_mask->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	texture_mask->setWrap(Texture::WRAP_S, Texture::REPEAT);
	texture_mask->setWrap(Texture::WRAP_T, Texture::REPEAT);
	texture_mask->setImage(mask);

	stateSet->setTextureAttributeAndModes(
		// use texture unit 0
		0, texture_tex1, StateAttribute::ON
		);
	stateSet->setTextureAttributeAndModes(
		// use texture unit 1
		1, texture_tex2, StateAttribute::ON
		);
	stateSet->setTextureAttributeAndModes(
		// use texture unit 1
		2, texture_mask, StateAttribute::ON
		);

	stateSet->addUniform(new Uniform("dirt_map", 0));
	stateSet->addUniform(new Uniform("grass_map", 1));
	stateSet->addUniform(new Uniform("track_mask", 2));

	Uniform* dirt_scale_uniform = new Uniform("dirt_scale", texSize);
	stateSet->addUniform(dirt_scale_uniform);

	Uniform* grass_scale_uniform = new Uniform("grass_scale", texSize);
	stateSet->addUniform(grass_scale_uniform);

	Uniform* mask_scale_uniform = new Uniform("mask_scale", texSize/10);
	stateSet->addUniform(mask_scale_uniform);

	// define a texture transformation matrix for rotating and moving the
	// race track against the vehicle
	//textureTransform = new TexMat();

	//textureTransform->setMatrix(Matrix::scale(1.0, 1.0, 1.0));
	//stateSet->setTextureAttribute(
	//	0, textureTransform
	//	);

	return geode;
}

Node *createTank() {
	// simply load the converted osg subtree
	Node *node = osgDB::readNodeFile("tank.osg");
	return node;
}

PositionAttitudeTransform *createLight() {
	// create a light
	Light *light = new Light();
	// each light must have a unique number
	light->setLightNum(0);
	// we set the light's position via a PositionAttitudeTransform object
	light->setPosition(Vec4(0.0, 0.0, 0.0, 1.0));
	light->setDiffuse( Vec4(1.0, 1.0, 1.0, 1.0));
	light->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setAmbient( Vec4(0.1, 0.1, 0.1, 1.0));
	light->setConstantAttenuation(0.1);
	light->setLinearAttenuation(0.0);
	light->setQuadraticAttenuation(0.0);

	LightSource *lightSource = new LightSource();
	lightSource->setLight(light);
	// enable the light for the entire scene
	lightSource->setLocalStateSetModes(StateAttribute::ON);
	lightSource->setStateSetModes(*sceneStateSet, StateAttribute::ON);

	lightTransform = new PositionAttitudeTransform();
	lightTransform->addChild(lightSource);
	lightTransform->setPosition(Vec3(0, 0, 5));

	return lightTransform;
}

Node *startup() {
	Group *scene = new Group();
	sceneStateSet = scene->getOrCreateStateSet();
	sceneStateSet->ref();

	// 这里添加 cessna 模型，表明这个方法是通过操作背景移动而实现“开动”坦克的，而非直接操作坦克。
	Node *node1 = osgDB::readNodeFile("cessna.osg");
	mt = new MatrixTransform;
	mt->addChild(createTank());
	//mt->addChild()
	scene->addChild(mt);

	scene->addChild(createTrack(400.0, 20.0));
	//scene->addChild(createTank());
	scene->addChild(createLight());
	return scene;
}

void update() {
	// simulate steering
	if (turnLeft) {
		// do left turn ...
		rot+=0.01;
		//direction=Vec2(-sin(rot),cos(rot));
		direction = Vec2(cos(rot), sin(rot)); // use X axis as the initial direction
	} else if (turnRight) {
		// do right turn ...
		rot-=0.01;
		//direction=Vec2(-sin(rot),cos(rot));
		direction=Vec2(cos(rot), sin(rot));
	} else {
		// go straight ahead ...
	}

	if (moveForward)
	{
		// move forward ...
		speed+=0.002;
		if(speed > 0.4)
			speed=0.4;

		pos=Vec3(pos.x()+direction.x()*speed,pos.y()+direction.y()*speed,0);
	} else if (moveBackward)
	{
		speed-=0.002;
		if(speed < -0.4)
			speed=-0.4;
		pos=Vec3(pos.x()+direction.x()*speed,pos.y()+direction.y()*speed,0);
		breakfact=-1;
	} else
	{
		if(speed > 0.0)
			speed-=0.005;
		else if(abs(speed) < 0.001)
			speed=0.0;
		else
			speed+=0.005;

		pos=Vec3(pos.x()+direction.x()*speed,pos.y()+direction.y()*speed,0);
	}

	//osg::Matrix Pivot = osg::Matrix::translate( Vec3d(-.5,-.5,0) );
	osg::Matrix R = osg::Matrix::rotate(rot-osg::PI_2, 0, 0, 1);
	//osg::Matrix RPivot = osg::Matrix::translate( Vec3d(.5,.5,0) );

	//osg::Matrix Rotation = Pivot * R * RPivot;

	matrix=R*Matrix::translate(pos);
	// do the texture transformation calculation here
	//textureTransform->setMatrix(matrix);
	mt->setMatrix(matrix);
}

int main() {
	Node *scene = startup();
	if (!scene) return 1;
	KeyboardHandler *keyboardHandler = new KeyboardHandler();

	osgViewer::Viewer viewer;
	viewer.addEventHandler(keyboardHandler);
	viewer.setSceneData(scene);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.setUpViewInWindow( 50, 50, 1024, 768 );
	viewer.realize();

	while (!viewer.done()) {
		viewer.frame();
		update(/*viewer.elapsedTime()*/);
	}
}
#endif

//#define  TEST_PIVOT 1
#ifdef TEST_PIVOT
int main()
{
	osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osgt");	// Not moved, original model
	osg::ref_ptr<osg::Node> cessna2 = osgDB::readNodeFile("new_cessna.3ds");	// moved model, the translate vector is (0, -15.1, 0). Now the boundingbox center is about at (0, -17, -2.6).
	osg::ref_ptr<osg::Node> cessna3 = osgDB::readNodeFile("new_cessna.3ds");	// moved model

	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> pat2 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> pat3 = new osg::PositionAttitudeTransform;

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> mt2 = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> mt3 = new osg::MatrixTransform;

	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
	osg::ref_ptr<osg::ShapeDrawable> shape2 = new osg::ShapeDrawable;
	osg::ref_ptr<osg::ShapeDrawable> shape3 = new osg::ShapeDrawable;

	osg::Vec3 movevector = osg::Vec3(0, -15.1, 0);
	osg::Vec3 bbcenter = osg::Vec3(0, -17, -2.6);

	shape->setShape( new osg::Sphere(osg::Vec3(), 1.0f) );
	shape2->setShape( new osg::Sphere(movevector, 1.0f) );
	shape3->setShape( new osg::Sphere(bbcenter, 1.0f) );

	shape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	shape2->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	shape3->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geode> geode2 = new osg::Geode;
	osg::ref_ptr<osg::Geode> geode3 = new osg::Geode;
	geode->addDrawable(shape);
	geode2->addDrawable(shape2);
	geode3->addDrawable(shape3);

	pat->addChild(cessna);
	pat2->addChild(cessna2);
	pat3->addChild(cessna3);

	mt->addChild(pat);
	mt2->addChild(pat2);
	mt3->addChild((pat3));

	{
		pat2->setPivotPoint(bbcenter);
		//pat3->setPivotPoint(bbcenter);
		//pat2->setAttitude(osg::Quat(osg::PI/3, osg::X_AXIS));

		pat2->setPosition(osg::Vec3(bbcenter));
		//pat2->setScale(osg::Vec3(0.7,0.7,0.7));

		//pat3->setAttitude(osg::Quat(osg::PI/3, osg::X_AXIS));

		//mt2->setMatrix(osg::Matrix::rotate(osg::Quat(osg::PI/3, osg::X_AXIS)));
		//mt2->setMatrix(osg::Matrix::translate(osg::Vec3(0, 0, 20)));
	}

	osg::ref_ptr<osg::Group> scene = new osg::Group;

	scene->addChild(mt.get());
	scene->addChild(mt2.get());
	scene->addChild(mt3.get());
	scene->addChild(geode);
	scene->addChild(geode2);
	scene->addChild(geode3);

	osgViewer::Viewer viewer;
	viewer.setSceneData(scene);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.setUpViewInWindow( 50, 50, 1024, 768 );
	viewer.run();
}
#endif

//#define TEST_CONSOLE_3 3

#ifdef TEST_CONSOLE_3
int main()
{
	char a;
std::cin>>a;
switch(a)
{
case'c':		std::cout << "wtf! - c"<<std::endl;
case'b':  std::cout<<"fuck b"<<std::endl;
default: return 1;
}

	switch(a)
	{
	case 'a': std::cout<<"fuck a"<<std::endl;
	case'b':  std::cout<<"fuck b"<<std::endl;
	default: break;
	}

	char m;
	std::cin>>m;
}
#endif

#ifdef ORIGINAL_TANK
StateSet *sceneStateSet;

// key controls
bool turnLeft, turnRight, moveForward, moveBackward;

TexMat *textureTransform;
PositionAttitudeTransform *lightTransform;

Matrix matrix;
Vec3 pos = Vec3(0,0,1);
Vec2 direction = Vec2(0,0.001);
float rot=0;
float breakfact=1;
float speed=0;

class KeyboardHandler: public GUIEventHandler {
public:
	virtual bool handle(
		const GUIEventAdapter& ea, GUIActionAdapter& aa
		) {
			bool newState;
			switch (ea.getEventType()) {
			case GUIEventAdapter::KEYDOWN:
				newState = true;
				break;
			case GUIEventAdapter::KEYUP:
				newState = false;
				break;
			default:
				return false;
			}
			switch (ea.getKey()) {
			case GUIEventAdapter::KEY_Left:
				turnLeft = newState;
				break;
			case GUIEventAdapter::KEY_Right:
				turnRight = newState;
				break;
			case GUIEventAdapter::KEY_Up:
				moveForward = newState;
				break;
			case GUIEventAdapter::KEY_Down:
				moveBackward = newState;
				break;
			default:
				return false;
			}
			return true;
	}
	virtual void accept(GUIEventHandlerVisitor& v) {
		v.visit(*this);
	};
};

Geode* createPlane(int size, int texSize) {
	// vertex array
	Vec3Array *vertexArray = new Vec3Array();
	vertexArray->push_back(Vec3(-size/2,-size/2,0));
	vertexArray->push_back(Vec3(-size/2,size/2,0));
	vertexArray->push_back(Vec3(size/2,size/2,0));
	vertexArray->push_back(Vec3(size/2,-size/2,0));

	// face array
	// give indices of vertices in counter-clockwise order
	DrawElementsUInt *faceArray = new DrawElementsUInt(
		PrimitiveSet::QUADS, 0
		);
	faceArray->push_back(0);
	faceArray->push_back(1);
	faceArray->push_back(2);
	faceArray->push_back(3);

	// normal array
	// give the normals on the cube's faces
	Vec3Array *normalArray = new Vec3Array();
	normalArray->push_back(Vec3(0, 0, +1));

	// normal index
	// assign each vertex a normal (similar to assigning colors)
	TemplateIndexArray<unsigned int, Array::UIntArrayType, 24, 4> *normalIndexArray;
	normalIndexArray = new TemplateIndexArray<
		unsigned int, Array::UIntArrayType, 24, 4
	>();
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);
	normalIndexArray->push_back(0);

	// texture coordinates
	osg::Vec2Array *texCoords = new osg::Vec2Array();
	texCoords->push_back(Vec2(0.0, 1.0));
	texCoords->push_back(Vec2(0.0, 0.0));
	texCoords->push_back(Vec2(1.0, 0.0));
	texCoords->push_back(Vec2(1.0, 1.0));

	Geometry *geometry = new Geometry();
	geometry->setVertexArray(vertexArray);
	geometry->addPrimitiveSet(faceArray);
	geometry->setNormalArray(normalArray);
	geometry->setNormalIndices(normalIndexArray);
	geometry->setNormalBinding(Geometry::BIND_PER_VERTEX);
	geometry->setTexCoordArray(0, texCoords);

	Geode *face = new Geode();
	face->addDrawable(geometry);

	return face;
}

Node *createTrack(double size, float texSize) {
	// create a plane
	Geode *geode;
	geode = createPlane(size, texSize);

	StateSet *stateSet = geode->getOrCreateStateSet();
	stateSet->ref();

	Program *programObject = new Program();
	Shader *vertexObject = new Shader(Shader::VERTEX);
	Shader *fragmentObject = new Shader(Shader::FRAGMENT);
	programObject->addShader(fragmentObject);
	programObject->addShader(vertexObject);

	vertexObject->loadShaderSourceFromFile("track.vert");
	fragmentObject->loadShaderSourceFromFile("track.frag");

	stateSet->setAttributeAndModes(programObject, StateAttribute::ON);

	Image *tex1 = osgDB::readImageFile("tex1.jpg");
	if (!tex1) {
		std::cout << "Couldn't load texture tex1.jpg" << std::endl;
		return NULL;
	}

	Image *tex2 = osgDB::readImageFile("tex2.jpg");
	if (!tex1) {
		std::cout << "Couldn't load texture tex2.jpg" << std::endl;
		return NULL;
	}

	Image *mask = osgDB::readImageFile("mask.jpg");
	if (!mask) {
		std::cout << "Couldn't load texture mask.jpg" << std::endl;
		return NULL;
	}

	Texture2D *texture_tex1 = new Texture2D;
	texture_tex1->setDataVariance(Object::DYNAMIC);
	texture_tex1->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture_tex1->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	texture_tex1->setWrap(Texture::WRAP_S, Texture::REPEAT);
	texture_tex1->setWrap(Texture::WRAP_T, Texture::REPEAT);
	texture_tex1->setImage(tex1);

	Texture2D *texture_tex2 = new Texture2D;
	texture_tex2->setDataVariance(Object::DYNAMIC);
	texture_tex2->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture_tex2->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	texture_tex2->setWrap(Texture::WRAP_S, Texture::REPEAT);
	texture_tex2->setWrap(Texture::WRAP_T, Texture::REPEAT);
	texture_tex2->setImage(tex2);

	Texture2D *texture_mask = new Texture2D;
	texture_mask->setDataVariance(Object::DYNAMIC);
	texture_mask->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture_mask->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	texture_mask->setWrap(Texture::WRAP_S, Texture::REPEAT);
	texture_mask->setWrap(Texture::WRAP_T, Texture::REPEAT);
	texture_mask->setImage(mask);

	stateSet->setTextureAttributeAndModes(
		// use texture unit 0
		0, texture_tex1, StateAttribute::ON
		);
	stateSet->setTextureAttributeAndModes(
		// use texture unit 1
		1, texture_tex2, StateAttribute::ON
		);
	stateSet->setTextureAttributeAndModes(
		// use texture unit 1
		2, texture_mask, StateAttribute::ON
		);

	stateSet->addUniform(new Uniform("dirt_map", 0));
	stateSet->addUniform(new Uniform("grass_map", 1));
	stateSet->addUniform(new Uniform("track_mask", 2));

	Uniform* dirt_scale_uniform = new Uniform("dirt_scale", texSize);
	stateSet->addUniform(dirt_scale_uniform);

	Uniform* grass_scale_uniform = new Uniform("grass_scale", texSize);
	stateSet->addUniform(grass_scale_uniform);

	Uniform* mask_scale_uniform = new Uniform("mask_scale", texSize/10);
	stateSet->addUniform(mask_scale_uniform);

	// define a texture transformation matrix for rotating and moving the
	// race track against the vehicle
	textureTransform = new TexMat();
	textureTransform->setMatrix(Matrix::scale(1.0, 1.0, 1.0));
	stateSet->setTextureAttribute(
		0, textureTransform
		);

	return geode;
}

Node *createTank() {
	// simply load the converted osg subtree
	Node *node = osgDB::readNodeFile("tank.osg");
	return node;
}

PositionAttitudeTransform *createLight() {
	// create a light
	Light *light = new Light();
	// each light must have a unique number
	light->setLightNum(0);
	// we set the light's position via a PositionAttitudeTransform object
	light->setPosition(Vec4(0.0, 0.0, 0.0, 1.0));
	light->setDiffuse( Vec4(1.0, 1.0, 1.0, 1.0));
	light->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setAmbient( Vec4(0.1, 0.1, 0.1, 1.0));
	light->setConstantAttenuation(0.1);
	light->setLinearAttenuation(0.0);
	light->setQuadraticAttenuation(0.0);

	LightSource *lightSource = new LightSource();
	lightSource->setLight(light);
	// enable the light for the entire scene
	lightSource->setLocalStateSetModes(StateAttribute::ON);
	lightSource->setStateSetModes(*sceneStateSet, StateAttribute::ON);

	lightTransform = new PositionAttitudeTransform();
	lightTransform->addChild(lightSource);
	lightTransform->setPosition(Vec3(0, 0, 5));

	return lightTransform;
}

Node *startup() {
	Group *scene = new Group();
	sceneStateSet = scene->getOrCreateStateSet();
	sceneStateSet->ref();
	scene->addChild(createTrack(400.0, 20.0));
	scene->addChild(createTank());
	scene->addChild(createLight());
	return scene;
}

void update(double time) {
	// simulate steering
	if (turnLeft) {
		// do left turn ...
		rot-=0.01;
		direction=Vec2(-sin(rot),cos(rot));
	} else if (turnRight) {
		// do right turn ...
		rot+=0.01;
		direction=Vec2(-sin(rot),cos(rot));
	} else {
		// go straight ahead ...
	}

	if (moveForward) {
		// move forward ...
		speed+=0.00001;
		if(speed > 0.001)
			speed=0.001;

		pos=Vec3(pos.x()-direction.x()*speed,pos.y()-direction.y()*speed,1);
	} else if (moveBackward) {
		speed-=0.00001;
		if(speed < -0.0005)
			speed=-0.0005;
		pos=Vec3(pos.x()-direction.x()*speed,pos.y()-direction.y()*speed,1);
		breakfact=-1;
	} else {
		if(speed > 0.0)
			speed-=0.00001;
		else if(abs(speed) < 0.00001)
			speed=0.0;
		else
			speed+=0.00001;

		pos=Vec3(pos.x()-direction.x()*speed,pos.y()-direction.y()*speed,1);
	}

	osg::Matrix Pivot = osg::Matrix::translate( Vec3d(-.5,-.5,0) );
	osg::Matrix R = osg::Matrix::rotate(rot, 0, 0, 1);
	osg::Matrix RPivot = osg::Matrix::translate( Vec3d(.5,.5,0) );

	osg::Matrix Rotation = Pivot * R * RPivot;

	matrix=Rotation*Matrix::translate(pos);
	// do the texture transformation calculation here
	textureTransform->setMatrix(matrix);
}

int main() {
	Node *scene = startup();
	if (!scene) return 1;
	KeyboardHandler *keyboardHandler = new KeyboardHandler();

	osgViewer::Viewer viewer;
	viewer.addEventHandler(keyboardHandler);
	viewer.setSceneData(scene);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.setUpViewInWindow( 50, 50, 1024, 768 );
	viewer.realize();

	while (!viewer.done()) {
		viewer.frame();
		update(viewer.elapsedTime());
	}
}
#endif