/* A Vector of 3 Controlled<T>s */
#ifndef _CONTROLLED_VECTOR3_
#define _CONTROLLED_VECTOR3_

#ifndef __STEPFLOW_TYPES__
#if !defined(__GNUC__)
typedef unsigned char byte;
#endif
#ifndef __VECTOR_INIT_
template<typename T>
struct Vector3{ T X, Y, Z; };
#endif
union data32 { unsigned char byte[4]; int i32; unsigned u32; float f32; };
#endif

#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW_NAMESPACE

#endif
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

    template<typename T=float>
    class Controlled;

    template<typename T=float>
    class ControlledVector3
    {
    public:
        Controlled<T> X;
        Controlled<T> Y;
        Controlled<T> Z;

        ControlledVector3(void)
        {
            initialize();
            ControllersActive(true);
        }

        void SetUp( Vector3<T> mins, Vector3<T> maxs, Vector3<T> movs,
                                     Vector3<T> vals, CtrlMode mode )
        {
            X.SetUp(mins.X,maxs.X,vals.X,movs.X,mode);
            X.SetUp(mins.Y,maxs.Y,vals.Y,movs.Y,mode);
            X.SetUp(mins.Z,maxs.Z,vals.Z,movs.Z,mode);
        }

        void SetMode(CtrlMode mode)
        {
            X.Mode(mode);
            Y.Mode(mode);
            Z.Mode(mode);
        }
        void SetMIN(Vector3<T> min)
        {
            X.MIN = min.X;
            Y.MIN = min.Y;
            Z.MIN = min.Z;
        }
        void SetMAX(Vector3<T> max)
        {
            X.MAX = max.X;
            Y.MAX = max.Y;
            Z.MAX = max.Z;
        }
        void SetMOVE(Vector3<T> move)
        {
            X.MOVE = move.X;
            Y.MOVE = move.Y;
            Z.MOVE = move.Z;
        }
        void SetCheckAtGet(bool checkedWhenGet)
        {
            X.SetCheckAtGet(checkedWhenGet);
            Y.SetCheckAtGet(checkedWhenGet);
            Z.SetCheckAtGet(checkedWhenGet);
        }
        void ControllersActive(bool setActive)
        {
            X.ControllerActive = setActive;
            Y.ControllerActive = setActive;
            Z.ControllerActive = setActive;
        }
        void ControllersActive(Vector3<bool> setActive)
        {
            X.ControllerActive = setActive.X;
            Y.ControllerActive = setActive.Y;
            Z.ControllerActive = setActive.Z;
        }
        Vector3<bool> Check(void)
        {
            return Vector3<bool>( X.Check(), Y.Check(), Z.Check() );
        }
        template<template<typename cT=T>class usermode> void SetUserMode(void)
        {
            X.SetUserMode<usermode>(X);
            Y.SetUserMode<usermode>(Y);
            Z.SetUserMode<usermode>(Z);
        }
        template<template<typename cT = T>class usermode> void SetUserMode(
            Vector3<T> min, Vector3<T> max, Vector3<T> mov, Vector3<T> val )
        {
            X.SetUserMode<usermode>(min.X,max.X,mov.X,val.X);
            Y.SetUserMode<usermode>(min.Y,max.Y,mov.Y,val.Y);
            Z.SetUserMode<usermode>(min.Z,max.Z,mov.Z,val.Z);
        }
        virtual operator Vector3<T>(void)
        {
            return Vector3<T>( X, Y, Z );
        }
        Vector3<T> operator =(Vector3<T> setter)
        {
            X = (T)setter.X; Y = (T)setter.Y; Z = (T)setter.Z;
            return{ X,Y,Z };
        }
        void SetVariables(T& refX,T& refY,T& refZ)
        {
            X.SetVariable(&refX);
            Y.SetVariable(&refY);
            Z.SetVariable(&refZ);
        }
    private:
        void initialize(void)
        { T mn, mx, mv,vl;
            if (std::numeric_limits<T>().is_integer) {
                mn = std::numeric_limits<T>().min();
                mx = std::numeric_limits<T>().max();
                mv = 1;
            }
            else {
                mv = std::numeric_limits<T>().epsilon();
                mn = -1; mx = 1; vl = 0;
                X.SetUp(mn, mx, vl, mv, Clamp);
                Y.SetUp(mn, mx, vl, mv, Clamp);
                Z.SetUp(mn, mx, vl, mv, Clamp);
            }
        }
    };

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW_NAMESPACE
#endif
#endif
