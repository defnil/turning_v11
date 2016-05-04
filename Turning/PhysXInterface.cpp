#include "StdAfx.h"
#include <extensions/PxVisualDebuggerExt.h>
#include <osg/ref_ptr>
#include "PhysXInterface.h"

using namespace physx;

#define PVD_HOST "127.0.0.1"

//class ErrorCallback: public PxErrorCallback
//{
//public:
//	virtual void reportError( PxErrorCode::Enum code, const char* message, const char* file, int line )
//	{
//		OSG_WARN << "Error " << code << " was found in (" << file << ", " << line << "): "
//			<< message << std::endl;
//	}
//};

//static ErrorCallback errorHandler;		// the physx Guide uses static PxDefaultErrorCallback gDefaultErrorCallback; ---> #include <extensions\PxDefaultErrorCallback.h>
static PxDefaultAllocator gDefaultAllocatorCallback;		// the physx Guide uses static PxDefaultAllocator gDefaultAllocatorCallback; ---> #include <extensions\PxDefaultErrorCallback.h>
static PxDefaultErrorCallback gDefaultErrorCallback;

//PhysXInterface* PhysXInterface::instance()
//{
//	static osg::ref_ptr<PhysXInterface> s_registry = new PhysXInterface;
//	return s_registry.get();
//}

//PhysXInterface::PhysXInterface()
//	:   _scene(NULL), _material(NULL)
//{
//	_foundation = PxCreateFoundation( PX_PHYSICS_VERSION, defaultAllocator, errorHandler );
//	if ( !_foundation )
//	{
//		OSG_WARN << "Unable to initialize PhysX foundation." << std::endl;
//		return;
//	}
//
//	// PxProfileZoneManager 做性能分析用
//	bool recordMemoryAllocations = true;
//	PxProfileZoneManager* mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(_foundation);
//	if(!mProfileZoneManager)
//		OSG_WARN << "PxProfileZoneManager::createProfileZoneManager failed." << std::endl;
//
//
//	// on windows, a GpuDispatcher to use for CUDA-accelerated features
//#ifdef PX_WINDOWS
//	pxtask::CudaContextManagerDesc cudaContextManagerDesc;
//	pxtask::CudaContextManager* _cudaContextManager = pxtask::createCudaContextManager(*_foundation, cudaContextManagerDesc, mProfileZoneManager);
//	if( _cudaContextManager )
//	{
//		if( !_cudaContextManager->contextIsValid() )
//		{
//			_cudaContextManager->release();
//			_cudaContextManager = NULL;
//		}
//	}
//#endif
//
//	_physicsSDK = PxCreatePhysics( PX_PHYSICS_VERSION, *_foundation, PxTolerancesScale(), recordMemoryAllocations, mProfileZoneManager );
//	if ( !_physicsSDK )
//	{
//		OSG_WARN << "Unable to initialize PhysX SDK." << std::endl;
//		return;
//	}
//
//	_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *_foundation, PxCookingParams());
//	if (!_cooking)
//	{
//		OSG_WARN << "Unable to initialize PhysX Cooking." << std::endl;
//	}
//
//
//	if ( !PxInitExtensions(*_physicsSDK) )
//	{
//		OSG_WARN << "Unable to initialize PhysX extensions." << std::endl;
//	}
//
//#ifdef _DEBUG
//	if ( _physicsSDK->getPvdConnectionManager() )
//	{
//		PxVisualDebuggerExt::createConnection(
//			_physicsSDK->getPvdConnectionManager(), "localhost", 5425, 10000 );
//	}
//	else
//	{
//		OSG_WARN << "Unable to start the PhysX visual debugger." << std::endl;
//	}
//#endif
//}

PhysXInterface::PhysXInterface() :
		mInitialDebugRender(true),
		mDebugRenderScale(1.0f),
		mCreateCudaCtxManager(false),
		mCreateGroundPlane(true),
		mNbThreads(1),
		mDefaultDensity(1.0f),
		mbStopDriving(false),

		mFoundation(NULL),
		mProfileZoneManager(NULL),
		mPhysics(NULL),
		mCooking(NULL),
		mScene(NULL),
		mMaterial(NULL),
		mCpuDispatcher(NULL),
#ifdef PX_WINDOWS
		mCudaContextManager(NULL)
#endif
	{
	}

PhysXInterface::~PhysXInterface()
{
		if(!_actors.empty() && _actors.begin()->second!=NULL)	// at least has a valid element (Map不为空不一定就是没有元素，可能是些无效值）
		{
			for ( ActorMap::iterator itr=_actors.begin(); itr!=_actors.end(); ++itr )
			{
				mScene->removeActor( *(itr->second) );
			}
			_actors.clear();
		}

		if(!_staticActors.empty() && _staticActors.begin()->second!=NULL)
		{
			for( StaticActorMap::iterator itr=_staticActors.begin(); itr!=_staticActors.end(); ++itr )
			{
				mScene->removeActor(*(itr->second) );
			}
			_staticActors.clear();
		}

		if(!_kinematicActors.empty() && _kinematicActors.begin()->second!=NULL)
		{
			for( ActorMap::iterator itr=_kinematicActors.begin(); itr!=_kinematicActors.end(); ++itr )
			{
				mScene->removeActor(*(itr->second) );
			}
			_kinematicActors.clear();
		}

		SAFE_RELEASE(mScene);
		SAFE_RELEASE(mCpuDispatcher);

		//	SAFE_RELEASE(mMaterial);
		SAFE_RELEASE(mCooking);
		PxCloseExtensions();

		//if(getApplication().getProfileZone())
		//{
		//	mProfileZoneManager->removeProfileZone(*getApplication().getProfileZone());
		//	getApplication().releaseProfileZone();
		//}
		SAFE_RELEASE(mPhysics);
#ifdef PX_WINDOWS
		SAFE_RELEASE(mCudaContextManager);
#endif
		SAFE_RELEASE(mProfileZoneManager);
		SAFE_RELEASE(mFoundation); // shutdown foundation after all physx objects shutdown
}

void PhysXInterface::createWorld( const osg::Plane& plane )
{
	// Create the ground
	osg::Quat q; q.makeRotate( osg::Vec3f(1.0f, 0.0f, 0.0f), osg::Vec3f(plane[0], plane[1], plane[2]) );		// infinite plane: ax+by+cz+d=0 (it is better if a^2+b^2+c^2=1 but not necessary) because a^2+b^2+c^2=1, d (d>=0) is the distance between the origin and the plane
	PxTransform pose( PxVec3(0.0f, 0.0f, plane[3]), PxQuat(q[0], q[1], q[2], q[3]) );
	PxRigidStatic* actor = mPhysics->createRigidStatic( pose );
	if ( !actor )
	{
		OSG_WARN <<"Failed to create the static plane." << std::endl;
		return;
	}

	PxShape* shape = actor->createShape( PxPlaneGeometry(), *mMaterial );
	if ( !shape )
	{
		OSG_WARN <<"Failed to create the plane shape." << std::endl;
		return;
	}
	mScene->addActor( *actor );
	//_staticActors
}

void PhysXInterface::createBox( int id, const PxVec3& pos, const PxVec3& dims, const PxVec3* linVel, PxReal density, bool kinematic)
{
	//PxBoxGeometry geometry( PxVec3(dim[0], dim[1], dim[2]) );
	PxRigidDynamic* box = PxCreateDynamic( *mPhysics, PxTransform(pos), PxBoxGeometry(dims), *mMaterial, density );
	ASSERT(box);
	if(!kinematic)	{ SetupDefaultRigidDynamic(*box, false); _actors[id] = box; }
	else
	{
		SetupDefaultRigidDynamic(*box, true); _kinematicActors[id] = box;

		PxShape* VehicleShape = box->createShape(PxBoxGeometry(dims), *getDefaultMaterial()); // to set word3 to vehicle
		setVehicle(*VehicleShape);
	}

	mScene->addActor(*box);
	if(linVel)
		box->setLinearVelocity(*linVel);
}

//void PhysXInterface::createKinematicBox( int id, const osg::Vec3& dim, double density )
//{
//	PxBoxGeometry geometry( PxVec3(dim[0], dim[1], dim[2]) );
//	PxRigidDynamic* kactor = PxCreateDynamic( *_physicsSDK, PxTransform::createIdentity(), geometry, *_material, density );
//	if ( kactor )
//	{
//		kactor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
//		_scene->addActor( *kactor );
//		_kinematicActors[id] = kactor;
//	}
//}
//
void PhysXInterface::createStaticBox( int id, const osg::Vec3& dim)		// position, direction info will be set in addphysicsdada() function
{
	PxBoxGeometry geometry( PxVec3(dim[0], dim[1], dim[2]) );
	PxRigidStatic* staticBoxActor = PxCreateStatic(*mPhysics, PxTransform::createIdentity(), geometry, *mMaterial);
	if ( staticBoxActor )
	{
		mScene->addActor(*staticBoxActor);
		_staticActors[id] = staticBoxActor;
	}
}
//
//void PhysXInterface::createWalls( int id )
//{
//	PxVec3 convexVerts[] = {PxVec3(0,10,0),PxVec3(10,0,0),PxVec3(-10,0,0),PxVec3(0,0,10),PxVec3(0,0,-10)};
//	PxConvexMeshDesc convexDesc;
//	convexDesc.points.count     = 5;
//	convexDesc.points.stride    = sizeof(PxVec3);
//	convexDesc.points.data      = convexVerts;
//	convexDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX;
//
//	PxToolkit::MemoryOutputStream buf;
//	if(!_cooking->cookConvexMesh(convexDesc, buf))
//	{
//		OSG_WARN << "Unable to create ConvexMesh Wall." << std::endl;
//		return;
//	}
//	PxToolkit::MemoryInputData input(buf.getData(), buf.getSize());
//	PxConvexMesh* convexMesh = _physicsSDK->createConvexMesh(input);
//
//	PxRigidStatic* wallActor = _physicsSDK->createRigidStatic(PxTransform::createIdentity());
//	PxShape* aConvexShape = wallActor->createShape(PxConvexMeshGeometry(convexMesh), *_material);
//
//	if ( wallActor )
//	{
//		_scene->addActor(*wallActor);
//		_staticActors[id] = wallActor;
//	}
//}
//
//void PhysXInterface::createSphere( int id, double radius, double density )
//{
//	PxSphereGeometry geometry( radius );
//	PxRigidDynamic* actor = PxCreateDynamic( *_physicsSDK, PxTransform::createIdentity(), geometry, *_material, density );
//	if ( actor )
//	{
//		actor->setAngularDamping( 0.75 );
//		_scene->addActor( *actor );
//		_actors[id] = actor;
//	}
//}
//
//void PhysXInterface::setVelocity( int id, const osg::Vec3& vec )
//{
//	PxRigidDynamic* actor = _actors[id];
//	if ( actor )
//		actor->setLinearVelocity( PxVec3(vec.x(), vec.y(), vec.z()) );
//}

void PhysXInterface::setMatrix( int id, const osg::Matrix& matrix )
{
	PxReal d[16];
	for ( int i=0; i<16; ++i ) d[i] = *(matrix.ptr() + i);

	PxRigidDynamic* actor = _actors[id];
	if ( actor ) actor->setGlobalPose( PxTransform(PxMat44(d)) );	// 利用数组 d 把 osg::Matrix 格式转换成 PhysX 中的 PxMat44 格式
}

void PhysXInterface::setMatrix_KinematicActor( int id, const osg::Matrix& matrix )
{
	PxReal d[16];
	for ( int i=0; i<16; ++i ) d[i] = *(matrix.ptr() + i);

	PxRigidDynamic* actor = _kinematicActors[id];
	if ( actor ) actor->setGlobalPose( PxTransform(PxMat44(d)) );	// for kinematic actors, the method setKinematicTarget() can also be used, they have differences, see user guide.
}

osg::Matrix PhysXInterface::getMatrix( int id )
{
	PxRigidDynamic* actor = _actors[id];
	if ( actor )
	{
		float m[16];
		PxMat44 pxMat( actor->getGlobalPose() );
		for ( int i=0; i<16; ++i ) m[i] = *(pxMat.front() + i);
		return osg::Matrix(&m[0]);
	}
	return osg::Matrix();
}

void PhysXInterface::simulate( double step )
{
	mScene->simulate( step );
	while( !mScene->fetchResults() ) { /* do nothing but wait */ }
}

void PhysXInterface::togglePvdConnection()
{
	if(!mPhysics->getPvdConnectionManager()) return;
	if (mPhysics->getPvdConnectionManager()->isConnected())
		mPhysics->getPvdConnectionManager()->disconnect();
	else
		createPvdConnection();
}

void PhysXInterface::createPvdConnection()
{
	if(mPhysics->getPvdConnectionManager() == NULL)
		return;

	//The connection flags state overall what data is to be sent to PVD.  Currently
	//the Debug connection flag requires support from the implementation (don't send
	//the data when debug isn't set) but the other two flags, profile and memory
	//are taken care of by the PVD SDK.

	//Use these flags for a clean profile trace with minimal overhead
	PxVisualDebuggerConnectionFlags theConnectionFlags( PxVisualDebuggerExt::getAllConnectionFlags() );

	//Create a pvd connection that writes data straight to the filesystem.  This is
	//the fastest connection on windows for various reasons.  First, the transport is quite fast as
	//pvd writes data in blocks and filesystems work well with that abstraction.
	//Second, you don't have the PVD application parsing data and using CPU and memory bandwidth
	//while your application is running.
	//PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(), "c:\\temp.pxd2", theConnectionFlags);

	//The normal way to connect to pvd.  PVD needs to be running at the time this function is called.
	//We don't worry about the return value because we are already registered as a listener for connections
	//and thus our onPvdConnected call will take care of setting up our basic connection state.
	PVD::PvdConnection* theConnection = PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(), PVD_HOST, 5425, 10, theConnectionFlags );
	if (theConnection)
		theConnection->release();
}

void PhysXInterface::onPvdConnected(PVD::PvdConnection& )
{
	//setup joint visualization.  This gets piped to pvd.
	mPhysics->getVisualDebugger()->setVisualizeConstraints(true);
	mPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::eTRANSMIT_CONTACTS, true);
}

void PhysXInterface::onInit()
{
	//Recording memory allocations is necessary if you want to
	//use the memory facilities in PVD effectively.  Since PVD isn't necessarily connected
	//right away, we add a mechanism that records all outstanding memory allocations and
	//forwards them to PVD when it does connect.

	//This certainly has a performance and memory profile effect and thus should be used
	//only in non-production builds.
	//bool recordMemoryAllocations = true;
	//const bool useCustomTrackingAllocator = true;

	PxAllocatorCallback* allocator = &gDefaultAllocatorCallback;

	//if(useCustomTrackingAllocator)
	//	allocator = getSampleAllocator();		//optional override that will track memory allocations

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, gDefaultErrorCallback);
	if(!mFoundation)
	{
		OSG_WARN << "PxCreateFoundation failed!" << std::endl;
		return;
	}

	mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
	if(!mProfileZoneManager)
	{
		OSG_WARN << "PxProfileZoneManager::createProfileZoneManager failed!" << std::endl;
		return;
	}

	if(mCreateCudaCtxManager)
	{
#ifdef PX_WINDOWS
		pxtask::CudaContextManagerDesc cudaContextManagerDesc;
		mCudaContextManager = pxtask::createCudaContextManager(*mFoundation, cudaContextManagerDesc, mProfileZoneManager);
		if( mCudaContextManager )
		{
			if( !mCudaContextManager->contextIsValid() )
			{
				mCudaContextManager->release();
				mCudaContextManager = NULL;
			}
		}
#endif
	}

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), false, mProfileZoneManager);
	if(!mPhysics)
	{
		OSG_WARN << "PxCreatePhysics failed!" << std::endl;
		return;
	}

	if(!PxInitExtensions(*mPhysics))
	{
		OSG_WARN << "PxInitExtensions failed!" << std::endl;
		return;
	}

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
	if(!mCooking)
	{
		OSG_WARN << "PxCreateCooking failed!" << std::endl;
		return;
	}

	togglePvdConnection();

	//if(mPhysics->getPvdConnectionManager())
	//	mPhysics->getPvdConnectionManager()->addHandler(*this);

	// setup default material...
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);
	if(!mMaterial)
	{
		OSG_WARN << "createMaterial failed!" << std::endl;
		return;
	}

	PX_ASSERT(NULL == mScene);

	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());

	getDefaultSceneDesc(sceneDesc);
	customizeSceneDesc(sceneDesc);

	if(!sceneDesc.cpuDispatcher)
	{
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(mNbThreads);
		if(!mCpuDispatcher)
		{
			OSG_WARN << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
			return;
		}
		sceneDesc.cpuDispatcher	= mCpuDispatcher;
	}

	if(!sceneDesc.filterShader)
		sceneDesc.filterShader	= getSampleFilterShader();

#ifdef PX_WINDOWS
	if(!sceneDesc.gpuDispatcher && mCudaContextManager)
	{
		sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	}
#endif

	//sceneDesc.flags |= PxSceneFlag::eENABLE_TWO_DIRECTIONAL_FRICTION;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;//need to register the pcm code in the PxCreatePhysics
	//sceneDesc.flags |= PxSceneFlag::eENABLE_ONE_DIRECTIONAL_FRICTION;
	//sceneDesc.flags |= PxSceneFlag::eADAPTIVE_FORCE;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

	mScene = mPhysics->createScene(sceneDesc);
	if(!mScene)
	{
		OSG_WARN << "createScene failed!" << std::endl;
		return;
	}

	mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE,				mInitialDebugRender ? mDebugRenderScale : 0.0f);
	mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES,	1.0f);

	//mApplication.refreshVisualizationMenuState(PxVisualizationParameter::eCOLLISION_SHAPES);
	//mApplication.applyDefaultVisualizationSettings();
	//mApplication.setMouseCursorHiding(false);
	//mApplication.setMouseCursorRecentering(false);
	//mCameraController.setMouseLookOnMouseButton(false);
	//mCameraController.setMouseSensitivity(1.0f);

	if(mCreateGroundPlane)
		createWorld();
}

void PhysXInterface::customizeSceneDesc(PxSceneDesc& sceneDesc)
{
	// Here we disable the gravity
	//sceneDesc.gravity	=		PxVec3(0, 0, -9.81);
	sceneDesc.filterShader						=	filter;
	sceneDesc.simulationEventCallback		=	this;
	//sceneDesc.flags									|= PxSceneFlag::eENABLE_SWEPT_INTEGRATION; // used for Continuous Collision Detection
	sceneDesc.flags									|= PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS;
}

void PhysXInterface::setCCDActive(PxShape& shape)
{
	shape.setFlag(PxShapeFlag::eUSE_SWEPT_BOUNDS,true);

	PxFilterData fd = shape.getSimulationFilterData();
	fd.word3 |= CCD_FLAG;
	shape.setSimulationFilterData(fd);
}

bool PhysXInterface::isCCDActive(PxFilterData& filterData)
{
	return filterData.word3 & CCD_FLAG ? true : false;
}

bool PhysXInterface::needsContactReport(const PxFilterData& filterData0, const PxFilterData& filterData1)
{
	const PxU32 needsReport = PxU32(DRIVINGSTOPPABLE_FLAG | VEHICLE_FLAG);	// 前者必为 wall，后者为车，则返回报告
	PxU32 flags = (filterData0.word3 | filterData1.word3);
	return (flags & needsReport) == needsReport;
}

void PhysXInterface::setVehicle(PxShape& shape)
{
	PxFilterData fd = shape.getSimulationFilterData();
	fd.word3 |= PxU32(VEHICLE_FLAG);
	shape.setSimulationFilterData(fd);
}

void PhysXInterface::setDrivingStoppable(PxShape& shape)
{
	PxFilterData fd = shape.getSimulationFilterData();
	fd.word3 |= PxU32(DRIVINGSTOPPABLE_FLAG);
	shape.setSimulationFilterData(fd);
}

bool PhysXInterface::isDrivingStoppable(PxFilterData& filterData)
{
	return filterData.word3 & PxU32(DRIVINGSTOPPABLE_FLAG) ? true : false;
}

PxFilterFlags PhysXInterface::filter(	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize)
{
	if (isCCDActive(filterData0) || isCCDActive(filterData1))
	{
		pairFlags |= PxPairFlag::eSWEPT_INTEGRATION_LINEAR;
	}

	if (needsContactReport(filterData0, filterData1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}

	pairFlags |= PxPairFlag::eCONTACT_DEFAULT;  // =eRESOLVE_CONTACTS
	return PxFilterFlags();
}

void PhysXInterface::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for(PxU32 i=0; i<nbPairs; i++)
	{
		PxU32 n = 2;
		const PxContactPair& cp = pairs[i];
		while(n--)
		{
			PxShape* shape = cp.shapes[n];
			PxFilterData fd = shape->getSimulationFilterData();
			if(isDrivingStoppable(fd))
			{
				TRACE("Collide!");
				mbStopDriving = true;	// 只要两个actor其中之一是墙就行（因为里面必定有一个是车，由needsConctReport 引发此函数可以知道）
			}
		}
	}
}