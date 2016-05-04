#pragma once
#ifndef H_PHYSXINTERFACE
#define H_PHYSXINTERFACE

#include <PxPhysicsAPI.h>
#include "PxTkStream.h"
#include "RendererMemoryMacros.h"
#include "ConsolePrinter.h"
#include <extensions/PxExtensionsAPI.h>
#include <PxSimulationEventCallback.h>
#include <PxShape.h>

#include <osg/Referenced>
#include <osg/Vec3>
#include <osg/Plane>
#include <osg/Matrix>
#include <string>
#include <map>

using namespace physx;

class PhysXInterface : public osg::Referenced,
	public PxSimulationEventCallback
{
	typedef std::map<int, physx::PxRigidDynamic*> ActorMap;	// ������ͳʹ��PxRigidActor, �磺PxRigidActor b, ����ʹ��ʱ��ʱ��Ҫת������ PxRigidDynamic *a = (PxRigidDynamic*) b
	typedef std::map<int, physx::PxRigidStatic*> StaticActorMap;

public:
	PhysXInterface();
	virtual ~PhysXInterface();
	//static PhysXInterface* instance();	// static member function is used for handling static variables
	physx::PxPhysics* getPhysics() { return mPhysics; }
	physx::PxScene* getScene() { return mScene; }
	physx::PxCooking* getCooking() { return mCooking; }
	physx::PxMaterial* getDefaultMaterial() { return mMaterial; }

	//StaticActorMap getStaticActorGroup() { return _staticActors; }
	//ActorMap getActorGroup() { return _actors; }
	//ActorMap getKinematicActorGroup() { return _kinematicActors; }

	//PxRigidActor* createRigidActor(	PxScene* scene, PxPhysics* physics,
	//	const PxTransform* pose, const PxGeometry* geometry, PxMaterial* material,
	//	const PxFilterData* fd, const PxReal* density, const PxReal* mass, PxU32 flags);

	void												createWorld(const osg::Plane& plane=osg::Plane(0.0f, 0.0f, 1.0f, 0.0f));
	void												createBox(int id, const PxVec3& pos, const PxVec3& dims, const PxVec3* linVel=NULL, PxReal density=1.0f, bool kinematic=false);
	/*PxRigidDynamic*							createSphere(const PxVec3& pos, PxReal radius, const PxVec3* linVel=NULL, RenderMaterial* material=NULL, PxReal density=1.0f);
	PxRigidDynamic*							createCapsule(const PxVec3& pos, PxReal radius, PxReal halfHeight, const PxVec3* linVel=NULL, RenderMaterial* material=NULL, PxReal density=1.0f);
	PxRigidDynamic*							createConvex(const PxVec3& pos, const PxVec3* linVel=NULL, RenderMaterial* material=NULL, PxReal density=1.0f);
	PxRigidDynamic*							createCompound(const PxVec3& pos, const std::vector<PxTransform>& localPoses, const std::vector<const PxGeometry*>& geometries, const PxVec3* linVel=NULL, RenderMaterial* material=NULL, PxReal density=1.0f);
	PxRigidDynamic*							createTestCompound(const PxVec3& pos, PxU32 nbBoxes, float boxSize, float amplitude, const PxVec3* linVel, RenderMaterial* material, PxReal density, bool makeSureVolumeEmpty = false);
	*/
	//void createWorld( const osg::Plane& plane, const osg::Vec3& gravity );
	//void createBox( int id, const osg::Vec3& dim, double mass );
	//void createKinematicBox( int id, const osg::Vec3& dim, double mass );
	 void createStaticBox( int id, const osg::Vec3& dim);
	//void createSphere( int id, double radius, double mass );
	//void createWalls( int id );

	//void setVelocity( int id, const osg::Vec3& pos );
	void setMatrix( int id, const osg::Matrix& matrix );
	void setMatrix_KinematicActor( int id, const osg::Matrix& matrix );
	osg::Matrix getMatrix( int id );

	void simulate( double step );

	void									onInit();
	void									onInit(bool restart) { onInit(); }
	void									getDefaultSceneDesc(PxSceneDesc&)  {}
	void									customizeSceneDesc(PxSceneDesc&);

	void									togglePvdConnection();
	void									createPvdConnection();
	void									onPvdConnected(PVD::PvdConnection& inFactory);

	static PxFilterFlags				filter(PxFilterObjectAttributes attributes0,
													PxFilterData filterData0,
													PxFilterObjectAttributes attributes1,
													PxFilterData filterData1,
													PxPairFlags& pairFlags,
													const void* constantBlock,
													PxU32 constantBlockSize);

	PX_FORCE_INLINE	PxSimulationFilterShader getSampleFilterShader() {	return PxDefaultSimulationFilterShader; }
	PX_FORCE_INLINE void SetupDefaultRigidDynamic(PxRigidDynamic& body, bool kinematic=false)
							{
								body.setActorFlag(PxActorFlag::eVISUALIZATION, true);
								body.setAngularDamping(0.5f);
								body.setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, kinematic);
							}

protected:

	bool									mInitialDebugRender;
	PxReal								mDebugRenderScale;
	bool									mCreateCudaCtxManager;
	bool									mCreateGroundPlane;
	PxU32								mNbThreads;

	PxReal								mDefaultDensity;
	PxFoundation*					mFoundation;
	PxPhysics*							mPhysics;
	PxCooking*						mCooking;
	PxScene*							mScene;
	PxMaterial*						mMaterial;

	PxDefaultCpuDispatcher*		mCpuDispatcher;
	pxtask::CudaContextManager*	mCudaContextManager;
	PxProfileZoneManager*					mProfileZoneManager;

public:

	// dynammics:
	enum
	{
		CCD_FLAG						= 1<<29,	// enable CCD
		VEHICLE_FLAG				= 1<<30,  // representing the object is vehicle type
		DRIVINGSTOPPABLE_FLAG		= 1<<31	// representing the events force the vehicle to stop
	};
	// Note: ������Щ key Ϊ int �͵� map ��ʵ��������vector���������ҿ��ܸ�����
	StaticActorMap _staticActors;		// static actors = Wall Actors
	ActorMap _actors;		// dynamic actors
	ActorMap _kinematicActors;	// dynamic & kinematic actors = Vehicle Actors

	bool			mbStopDriving;	// ������Ϣ���� osg��true: �ó�ֹͣǰ��.

	static	void							setVehicle(PxShape& shape);
	static	void							setDrivingStoppable(PxShape& shape); // Ŀǰ��Ҫ�Ǹ� wall �������Եģ������������Ǹ�����Ŧ�۵Ȳ�λ���� detachable һ��
	static	bool							isDrivingStoppable(PxFilterData& filterData); // ֻҪ��ǽ�ͳ�������ײ����ͣ��actor
	static	bool							needsContactReport(const PxFilterData& filterData0, const PxFilterData& filterData1);
	static	void							setCCDActive(PxShape& shape);
	static	bool							isCCDActive(PxFilterData& filterData);

	// Implements PxSimulationEventCallback
	virtual void							onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual void							onTrigger(PxTriggerPair*, PxU32) {}
	virtual void							onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void							onWake(PxActor** , PxU32 ) {}
	virtual void							onSleep(PxActor** , PxU32 ){}
	//physx::PxFoundation* _foundation;
	//physx::PxPhysics* _physicsSDK;
	//physx::PxScene* _scene;
	//physx::PxMaterial* _material;
	//physx::PxCooking* _cooking;
	//physx::pxtask::CudaContextManager* _cudaContextManager;
};

#endif
