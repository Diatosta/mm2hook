#pragma once
#include "mm2_audio.h"
#include "mm2_common.h"
#include "mm2_inst.h"
#include "mm2_game.h"
#include "mm2_breakable.h"

# define M_PI          3.141592653589793238462643383279502884L /* pi */

namespace MM2
{
    // Forward declarations
    class mmVehInfo;
    class mmVehList;
    class vehCar;
	class vehTransmission;
	class vehCarSim;
    class vehCarAudio;
    class vehCarAudioContainer;
    class vehPoliceCarAudio;
    class vehTrailer;
    class vehTrailerInstance;

    class mmVehInfo {
    private:
        char baseName[40];
        char description[80];
        char colors[64];
        int flags;
        int order;
        BOOL valid;
        int unlockScore;
        int unlockFlags;
        BOOL isLocked;
        int horsepower;
        int topSpeed;
        int durability;
        int mass;
        int rewardFlags;
        int unk_10C;
        int scoringBias;
        float uiDist;
        float unk_118;
        float unk_11C;
        unsigned long tuningCRC;
    public:
        AGE_API mmVehInfo(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x529720>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~mmVehInfo(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x529770>::Call<void>(this);
            POP_VTABLE();
        }

        inline char * GetBaseName(void) {
            return baseName;
        }

        inline char * GetDescription(void) {
            return description;
        }
    };

    class mmVehList {
    private:
        mmVehInfo **vehInfos;
        mmVehInfo *defaultVehicle;
        int numVehicles;
    public:
        AGE_API mmVehList(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x524550>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~mmVehList(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x524570>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API int GetVehicleID(char *vehicle)             { return ageHook::Thunk<0x5246B0>::Call<int>(this, vehicle); }

        AGE_API mmVehInfo * GetVehicleInfo(int vehicle)     { return ageHook::Thunk<0x5245E0>::Call<mmVehInfo *>(this, vehicle); }
        AGE_API mmVehInfo * GetVehicleInfo(char *vehicle)   { return ageHook::Thunk<0x524610>::Call<mmVehInfo *>(this, vehicle); }
    };

    declhook(0x6B1CA8, _Type<mmVehList *>, VehicleListPtr);

    class vehCarModel : public lvlInstance {
    protected:
        ageHook::Field<0xAC, vehBreakableMgr *> _mechanicalBreakableMgr;
        ageHook::Field<0xA0, vehBreakableMgr *> _genericBreakableMgr;
        ageHook::Field<0x18, vehCar *> _car;
    public:
        AGE_API vehCarModel()                               { ageHook::Thunk<0x4CCF20>::Call<void>(this); }
        AGE_API ~vehCarModel()                              { ageHook::Thunk<0x4CCF80>::Call<void>(this); }

        inline vehBreakableMgr * getGenBreakableMgr(void) const {
            return _genericBreakableMgr.get(this);
        }
        
        inline vehBreakableMgr * getMechBreakableMgr(void) const {
            return _mechanicalBreakableMgr.get(this);
        }

        inline vehCar * getCar(void) const {
            return _car.get(this);
        }

        AGE_API void BreakElectrics(Vector3* a1)            { ageHook::Thunk<0x4CEFE0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { ageHook::Thunk<0x4CDFF0>::Call<void>(this); }
        AGE_API void EjectOneshot()                         { ageHook::Thunk<0x4CDCA0>::Call<void>(this); }
        AGE_API bool GetVisible()                           { return ageHook::Thunk<0x4CF070>::Call<bool>(this); }
        AGE_API void SetVisible(bool a1)                    { ageHook::Thunk<0x4CF050>::Call<void>(this, a1); }
        
        /*
            lvlInstance virtuals
        */

        AGE_API void Reset()  override                      { ageHook::Thunk<0x4CDFD0>::Call<void>(this); }
        AGE_API Vector3 const & GetPosition() override
                                                            { return ageHook::Thunk<0x4CEF50>::Call<Vector3 const &>(this); }
        AGE_API Matrix34 const & GetMatrix(Matrix34* a1)
                                                            { return ageHook::Thunk<0x4CEF90>::Call<Matrix34 const &>(this, a1); }
        AGE_API void SetMatrix(const Matrix34* a1) override 
                                                            { ageHook::Thunk<0x4CEFA0>::Call<void>(this, a1); }
        AGE_API dgPhysEntity* GetEntity() override          { return ageHook::Thunk<0x4CEFC0>::Call<dgPhysEntity*>(this); }
        AGE_API dgPhysEntity* AttachEntity() override 
                                                            { return ageHook::Thunk<0x4CEFD0>::Call<dgPhysEntity*>(this); }
        AGE_API Vector3 const * GetVelocity() override
                                                            { return ageHook::Thunk<0x4CEF80>::Call<Vector3 const*>(this); }

        AGE_API void Draw(int a1) override                  { ageHook::Thunk<0x4CE040>::Call<void>(this, a1); }
        AGE_API void DrawShadow() override                  { ageHook::Thunk<0x4CE940>::Call<void>(this); }
        AGE_API void DrawShadowMap() override               { ageHook::Thunk<0x4CEA90>::Call<void>(this); }
        AGE_API void DrawGlow() override                    { ageHook::Thunk<0x4CEB90>::Call<void>(this); }
        AGE_API void DrawReflected(float a1) override
                                                            { ageHook::Thunk<0x4CF080>::Call<void>(this, a1); }
        AGE_API unsigned int SizeOf() override              { return ageHook::Thunk<0x4CDFE0>::Call<int>(this); }



        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCarModel, lvlInstance>("vehCarModel")
                //properties
                .addPropertyReadOnly("Breakables", &getGenBreakableMgr)
                .addPropertyReadOnly("WheelBreakables", &getMechBreakableMgr)

                //functions
                .addFunction("GetPosition", &GetPosition)
                .addFunction("Reset", &Reset)
                .addFunction("BreakElectrics", &BreakElectrics)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("EjectOneshot", &EjectOneshot)
                .addFunction("GetVisible", &GetVisible)
                .addFunction("SetVisible", &SetVisible)


            .endClass();
        }
    private:
        byte _buffer[0xCC];
    };

    class vehTransmission : public asNode {
    protected:
		ageHook::Field<0x18, vehCarSim *> _vehCarSim;
		ageHook::Field<0x1C, int> _field1C;
		ageHook::Field<0x20, BOOL> _automatic;
        ageHook::Field<0x24, int> _gear;
		ageHook::Field<0x28, int> _field28;
		ageHook::Field<0x54, int> _autoNumGears;
    public:
		inline vehCarSim * getVehCarSim(void) {
			return _vehCarSim.get(this);
		}

		inline void setVehCarSim(vehCarSim * vehCarSimToSet) {
			_vehCarSim.set(this, vehCarSimToSet);
		}
		inline int getField1C(void) {
			return _field1C.get(this);
		}
		inline void setField1C(int field1C) {
			_field1C.set(this, field1C);
		}
		inline BOOL getAutomatic(void) {
			return _automatic.get(this);
		}
		inline void setAutomatic(BOOL automatic) {
			_automatic.set(this, automatic);
		}
        inline int getGear(void) {
            return _gear.get(this);
        };
		inline void setGear(int gear) {
			_gear.set(this, gear);
		}
		inline void setField28(int field28) {
			_field28.set(this, field28);
		}
		inline int getAutoNumGears(void) {
			return _autoNumGears.get(this);
		};
		inline void setAutoNumGears(int autoNumGears) {
			_autoNumGears.set(this, autoNumGears);
		}

		//This is used in GearToRatioMPH, not sure what it stands for
		const float rationToMPH = 26.82240;

		int field0;
		int field4;
		int field8;
		int fieldC;
		int field10;
		int field14;
		//int field18;
		//int field1C;
		/*BOOL _automatic;
		int _currentGear;*/
		//int field28;
		float _gearChangeTime;
		int field30;
		int field34;
		int field38;
		int field3C;
		int field40;
		int field44;
		int field48;
		int field4C;
		int _manualNumGears;
		//int _autoNumGears;
		int field58;
		int field5C;
		int field60;
		int field64;
		int field68;
		int field6C;
		int field70;
		int field74;
		int field78;
		int field7C;
		int field80;
		int field84;
		int field88;
		int field8C;
		int field90;
		int field94;
		int field98;
		int field9C;
		int fieldA0;
		int fieldA4;
		int fieldA8;
		int fieldAC;
		int fieldB0;
		int fieldB4;
		int fieldB8;
		int fieldBC;
		int fieldC0;
		int fieldC4;
		int fieldC8;
		int fieldCC;
		int fieldD0;
		int fieldD4;
		float _reverse;
		float _low;
		float _high;
		float _upshiftBias;
		float _downshiftBiasMin;
		float _downshiftBiasMax;
		float _gearBias;

		/*vehTransmission() {
			int v2;

			asNode::asNode();
			this->field30 = -1054867456;
			this->field58 = -1054867456;
			v2 = 2;
			this->field18 = 0;
			this->field78 = 0;
			this->field98 = 0;
			this->fieldB8 = 0;
			this->field34 = 0;
			this->field5C = 0;
			this->field7C = 0;
			this->field9C = 0;
			this->fieldBC = 0;
			//this->field0 = this->vftable;
		}*/

		void Reset() {
			asNode::Reset();
			this->SetCurrentGear(2);
			this->setField28(0);
		}

		void ComputeConstants(double a2) {
			//Code
		}

		/*float GearRatioFromMPH(float a2) {
			float temp = rationToMPH * a2;
			float temp2 = *(getVehCarSim()->getField3FC() + 444);
			LogFile::Format("temp2 = %f\n", temp2);
			temp2 = temp2 * (M_PI * 2);
			temp = temp / temp2;
			float temp3 = getVehCarSim()->getField27E();
			return temp3;
		}*/

		void Init(vehCarSim * vehCarSimToSet) {
			this->setVehCarSim(vehCarSimToSet);
		}

		//Upshifts the car's transmission
		void Upshift() {
			int currentGear;

			if (this->getAutomatic()) {
				bool _automatic = this->getAutomatic() == 1;
				currentGear = this->getGear();
				if (currentGear < 2) {
					this->SetCurrentGear(currentGear + 1);
				}
			}
			else {
				currentGear = this->getGear();
				if (currentGear < (this->_manualNumGears) - 1) {
					this->SetCurrentGear(currentGear + 1);
				}
			}
		}

		//Downshifts the car's transmission
		void Downshift() {
			int currentGear;

			if (this->getAutomatic()) {
				currentGear = this->getGear();
				if (currentGear >= 2) {
					this->SetCurrentGear(1);
					return;
				}
				if (currentGear >= 1) {
					this->SetCurrentGear(0);
				}
			}
			else {
				currentGear = this->getGear();

				//Only downshift if we're above reverse
				if (currentGear > 0) {
					this->SetCurrentGear(currentGear - 1);
				}
			}
		}

		//Sets the transmission type
		void Automatic(BOOL isAutomatic) {
			this->setAutomatic(isAutomatic);
		}

		void SetReverse() {
			this->SetCurrentGear(0);
		}

		void SetNeutral() {
			this->SetCurrentGear(1);
		}

		void SetForward() {
			int currentGear = this->getGear();

			if (currentGear <= 1) {
				this->SetCurrentGear(2);
			}
		}

		void SetCurrentGear(int gearToSet) {
			if (gearToSet != this->getGear() && (!this->getAutomatic() || gearToSet < this->getAutoNumGears())) {
				this->setField28(0);
				this->setField1C(1);
				this->setGear(gearToSet);
			}
		}

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4CF600>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4CF740>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CF880>::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTransmission, asNode>("vehTransmission")
                .addProperty("Gear", &getGear, &SetCurrentGear)
                .addFunction("Automatic", &Automatic, LUA_ARGS(bool))
                .addFunction("Downshift", &Downshift)
                .addFunction("Upshift", &Upshift)
                .addFunction("SetReverse", &SetReverse)
                .addFunction("SetNeutral", &SetNeutral)
                .addFunction("SetForward", &SetForward)
                .addFunction("SetCurrentGear", &SetCurrentGear)
                //.addFunction("GearRatioFromMPH", &GearRatioFromMPH)
                .endClass();
        }
    private:
        //byte _buffer[0x4C];
    };

    class vehCarSim : public asNode {
    protected:
        ageHook::Field<0x24C, float> _speed;
        ageHook::Field<0x210, const Vector3> _resetPosition;
        ageHook::Field<0x1D0, lvlInstance *> _instance;
        ageHook::Field<0x2E0, vehTransmission> _transmission;
        //ageHook::Field<0x25C, vehEngine *> _engine;
		ageHook::Field<0x27E, float> _field27E;
		ageHook::Field<0x3FC, int *> _field3FC;
    public:
        inline float getSpeed(void) {
            return _speed.get(this);
        };

        inline const Vector3 getResetPosition(void) {
            return _resetPosition.get(this);
        }

        inline lvlInstance* getInstance(void) {
            return _instance.get(this);
        }

        inline vehTransmission* getTransmission(void) const {
            return _transmission.ptr(this);
        }
		inline float getField27E(void) {
			return _field27E.get(this);
		}
		inline int * getField3FC(void) {
			return _field3FC.get(this);
		}

        AGE_API vehCarSim()                                 { ageHook::Thunk<0x4CB660>::Call<void>(this); }
        AGE_API ~vehCarSim()                                { ageHook::Thunk<0x4CB8E0>::Call<void>(this); }

        AGE_API int BottomedOut()                           { return ageHook::Thunk<0x4CBB40>::Call<int>(this); }
        AGE_API int OnGround()                              { return ageHook::Thunk<0x4CBB00>::Call<int>(this); }
        AGE_API void ReconfigureDrivetrain()                { ageHook::Thunk<0x4CC0B0>::Call<void>(this); }
        AGE_API void SetHackedImpactParams()                { ageHook::Thunk<0x4CC080>::Call<void>(this); }
        AGE_API void RestoreImpactParams()                  { ageHook::Thunk<0x4CC050>::Call<void>(this); }
        AGE_API void SetResetPos(Vector3 * a1)              { ageHook::Thunk<0x4CC830>::Call<void>(this, a1); }
        
        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4CC8E0>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CBA70>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4CCC70>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CCF10>::Call<char*>(this); }
        AGE_API char const* GetDirName() override           { return ageHook::Thunk<0x4CBAF0>::Call<char const*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCarSim, asNode>("vehCarSim")
                .addPropertyReadOnly("Transmission", &getTransmission)
                .addPropertyReadOnly("ResetPosition", &getResetPosition)
                .addFunction("Reset", &Reset)
                .addFunction("BottomedOut", &BottomedOut)
                .addFunction("OnGround", &OnGround)
                .addFunction("ReconfigureDrivetrain", &ReconfigureDrivetrain)
                .addFunction("SetHackedImpactParams", &SetHackedImpactParams)
                .addFunction("RestoreImpactParams", &RestoreImpactParams)
                .addFunction("SetResetPos", &SetResetPos)
                .endClass();
        }
    private:
        byte _buffer[0x1560];
    };

    class vehCarDamage : public asNode {
    public:
        AGE_API vehCarDamage()                              { ageHook::Thunk<0x4CA380>::Call<void>(this); }
        AGE_API ~vehCarDamage()                             { ageHook::Thunk<0x4CA530>::Call<void>(this); }

        AGE_API void AddDamage(float a1)                    { ageHook::Thunk<0x4CAEC0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { ageHook::Thunk<0x4CAE80>::Call<void>(this); }

        /*
            vehCarDamage virtuals
        */

        virtual AGE_API float GetDamageModifier(phCollider* a1) 
                                                            { return ageHook::Thunk<0x4CB650>::Call<float>(this, a1); }
        
        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x5B2C30>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CAE60>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override
                                                            { ageHook::Thunk<0x4CB400>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CB640>::Call<char*>(this); }
        AGE_API char const* GetDirName() override           { return ageHook::Thunk<0x4CA5F0>::Call<char const*>(this); }
        

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarDamage>("vehCarDamage")
                .addFunction("Reset", &Reset)
                .addFunction("AddDamage", &AddDamage)
                .addFunction("ClearDamage", &ClearDamage)
            .endClass();
        }
    private:
        byte _buffer[0x3A4];
    };
    
    class vehSplash : public asNode {
    public:
        AGE_API vehSplash()                                 { ageHook::Thunk<0x4D6A00>::Call<void>(this); }
        AGE_API ~vehSplash()                                { ageHook::Thunk<0x4D6F30>::Call<void>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4D6BF0>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4D6A70>::Call<void>(this); }
    };

    class vehCar : public dgPhysEntity {
        // vehCarSim: 0xB8 (size: ~0x1560)
    protected:
        ageHook::Field<0xC0, vehCarDamage *> _damage;
        ageHook::Field<0xB8, vehCarSim *> _sim;
        ageHook::Field<0xBC, vehCarModel *> _model;
        ageHook::Field<0xE0, vehSplash *> _splash;
        ageHook::Field<0x254, vehCarAudioContainer *> _audio;
        ageHook::Field<0xD8, vehTrailer *> _trailer;
    public:
        AGE_API vehCar(BOOL a1)                             { ageHook::Thunk<0x42BAB0>::Call<void>(this, a1); }
        AGE_API ~vehCar()                                   { ageHook::Thunk<0x42BCC0>::Call<void>(this); }

        static ageHook::Type<bool> sm_DrawHeadlights;

        inline vehCarDamage* getCarDamage(void) const {
            return _damage.get(this);
        };

        inline vehCarSim* getCarSim(void) const {
            return _sim.get(this);
        }

        inline vehCarModel* getModel(void) const {
            return _model.get(this);
        }

        inline vehSplash* getSplash(void) const {
            return _splash.get(this);
        }

        inline vehCarAudioContainer* getAudio(void) const {
            return _audio.get(this);
        }

        inline vehTrailer* getTrailer(void) const {
            return _trailer.get(this);
        }

        AGE_API void Reset()                                { ageHook::Thunk<0x42C330>::Call<void>(this); }

        AGE_API void ClearDamage()                          { ageHook::Thunk<0x42C450>::Call<void>(this); }
        AGE_API bool IsPlayer()                             { return ageHook::Thunk<0x42C890>::Call<bool>(this); }
        AGE_API void Init(char const* a1, int a2, int a3, bool a4, bool a5)
                                                            { ageHook::Thunk<0x42BE10>::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void InitAudio(char const* a1, int a2)      { ageHook::Thunk<0x42C1F0>::Call<void>(this, a1, a2); }
        AGE_API void SetDrivable(BOOL a1, BOOL a2)          { ageHook::Thunk<0x42C2C0>::Call<void>(this, a1, a2); }

        /*
            dgPhysEntity virtuals
        */

        AGE_API bool RequiresTerrainCollision() override
                                                            { return ageHook::Thunk<0x42CA90>::Call<bool>(this); }
        AGE_API lvlInstance* GetInst() override             { return ageHook::Thunk<0x42CA80>::Call<lvlInstance *>(this); }
        AGE_API phInertialCS* GetICS()                      { return ageHook::Thunk<0x42CA70>::Call<phInertialCS *>(this); }
        AGE_API void PostUpdate() override                  { ageHook::Thunk<0x42C8B0>::Call<void>(this); }
        AGE_API void Update() override                      { ageHook::Thunk<0x42C690>::Call<void>(this); }
        AGE_API void PreUpdate() override                   { ageHook::Thunk<0x42C480>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCar, dgPhysEntity>("vehCar")
                .addConstructor(LUA_ARGS(bool))
                //properties
                .addPropertyReadOnly("vehCarDamage", &getCarDamage)
                .addPropertyReadOnly("vehCarSim", &getCarSim)
                .addPropertyReadOnly("vehCarModel", &getModel)
                .addPropertyReadOnly("vehSplash", &getSplash)
                .addPropertyReadOnly("Audio", &getAudio)
                .addPropertyReadOnly("Trailer", &getTrailer)

                //functions
                .addFunction("Init", &Init)
                .addFunction("InitAudio", &InitAudio)
                .addFunction("Reset", &Reset)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("SetDrivable", &SetDrivable, LUA_ARGS(bool,bool))
                .addFunction("IsPlayer", &IsPlayer)
            .endClass();
        }
    private:
        byte _buffer[0x25C];
    };

    class vehTrailer : public dgPhysEntity, public asNode {
    protected:
        ageHook::Field<0x1E4, vehCarSim *> _sim;
        ageHook::Field<0xCC, dgTrailerJoint> _joint;
    public:
        inline vehCarSim* getCarSim(void) const {
            return _sim.get(this);
        }

        inline dgTrailerJoint* getJoint(void) const {
            return _joint.ptr(this);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailer, dgPhysEntity>("vehTrailer")
                .addPropertyReadOnly("vehCarSim", &getCarSim)
                .addPropertyReadOnly("Joint", &getJoint)
                .endClass();
        }
    };

    class vehTrailerInstance : public lvlInstance {
    protected:
        ageHook::Field<0x14, vehTrailer *> _trailer;
    public:
        inline vehTrailer* getTrailer(void) const {
            return _trailer.get(this);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailerInstance, lvlInstance>("vehTrailerInstance")
                //properties
                .addPropertyReadOnly("Trailer", &getTrailer)
                .endClass();
        }
    };

    class vehCarAudio {
    private:
        byte _buffer[0x130];
    protected:
        ageHook::Field<0x118, vehCarSim *> _sim;
    public:
        inline vehCarSim* getCarSim(void) const {
            return _sim.get(this);
        };

    };

    class vehCarAudioContainer {
    public:
        AGE_API static bool IsPolce(const char* name) {
            return ageHook::StaticThunk<0x4D1A70>::Call<bool>(name);
        }
        
        AGE_API static void RegisterPoliceNames(Stream* a1, LPCSTR a2) {
            ageHook::StaticThunk<0x4D0F10>::Call<void>(a1, a2);
        }

        AGE_API static void SetSirenCSVName(LPCSTR name) {
            ageHook::StaticThunk<0x4D0C80>::Call<void>(name);
        };

        AGE_API void Set3D(bool a1) {
            ageHook::Thunk<0x4D1840>::Call<void>(this, a1);
        }

        AGE_API vehPoliceCarAudio * GetPoliceCarAudioPtr() {
            return ageHook::Thunk<0x4D1790>::Call<vehPoliceCarAudio *>(this);
        };

        AGE_API AudImpact * GetAudImpactPtr() {
            return ageHook::Thunk<0x4D1730>::Call<AudImpact *>(this);
        };
    };

    class vehPoliceCarAudio {
    public:
        static ageHook::Type<int> iNumCopsPursuingPlayer;
    };

    template<>
    void luaAddModule<module_vehicle>(LuaState L) {
        luaBind<vehCar>(L);
        luaBind<vehCarDamage>(L);
        luaBind<vehCarModel>(L);
        luaBind<vehCarSim>(L);
        luaBind<vehTransmission>(L);
        luaBind<vehTrailer>(L);
        luaBind<vehTrailerInstance>(L);
    }
}