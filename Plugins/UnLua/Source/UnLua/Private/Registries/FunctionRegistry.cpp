﻿#include "FunctionRegistry.h"

#include "lauxlib.h"
#include "LuaEnv.h"

namespace UnLua
{
    FFunctionRegistry::FFunctionRegistry(FLuaEnv* Env)
        : Env(Env)
    {
    }

    void FFunctionRegistry::NotifyUObjectDeleted(UObject* Object)
    {
        if (!LuaFunctions.Contains((ULuaFunction*)Object))
            return;

        FFunctionInfo Info;
        LuaFunctions.RemoveAndCopyValue((ULuaFunction*)Object, Info);
        luaL_unref(Env->GetMainState(), LUA_REGISTRYINDEX, Info.LuaRef);
    }

    void FFunctionRegistry::Invoke(ULuaFunction* Function, UObject* Context, FFrame& Stack, RESULT_DECL)
    {
        // TODO: refactor
        if (!Env->GetObjectRegistry()->IsBound(Context))
            Env->TryBind(Context);

        const auto L = Env->GetMainState();
        const auto SelfRef = Env->GetObjectRegistry()->GetBoundRef(Context);
        lua_Integer FuncRef;
        FFunctionDesc* FuncDesc;

        const auto Exists = LuaFunctions.Find(Function);
        if (Exists)
        {
            FuncRef = Exists->LuaRef;
            FuncDesc = Exists->Desc.Get();
        }
        else
        {
            FuncRef = LUA_NOREF;
            FuncDesc = new FFunctionDesc(Function, nullptr);

            lua_rawgeti(L, LUA_REGISTRYINDEX, SelfRef);
            lua_getmetatable(L, -1);
            do
            {
                lua_pushstring(L, FuncDesc->GetLuaFunctionName());
                lua_rawget(L, -2);
                if (lua_isfunction(L, -1))
                {
                    lua_pushvalue(L, -3);
                    lua_remove(L, -3);
                    lua_remove(L, -3);
                    lua_pushvalue(L, -2);
                    FuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
                    break;
                }
                lua_pop(L, 1);
                lua_pushstring(L, "Super");
                lua_rawget(L, -2);
                lua_remove(L, -2);
            }
            while (lua_istable(L, -1));
            lua_pop(L, 2);
            
            FFunctionInfo Info;
            Info.LuaRef = FuncRef;
            Info.Desc = TUniquePtr<FFunctionDesc>(FuncDesc);
            LuaFunctions.Add(Function, MoveTemp(Info));
        }

        check(FuncRef != LUA_NOREF);
        FuncDesc->CallLua(L, FuncRef, SelfRef, Stack, RESULT_PARAM);
    }
}