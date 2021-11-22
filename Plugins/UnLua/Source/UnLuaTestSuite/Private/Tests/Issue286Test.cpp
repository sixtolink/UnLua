// Tencent is pleased to support the open source community by making UnLua available.
// 
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the MIT License (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and limitations under the License.

#include "UnLuaTestCommon.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#if WITH_DEV_AUTOMATION_TESTS

struct FUnLuaTest_Issue286 : FUnLuaTestBase
{
    virtual FString GetMapName() override { return TEXT("/Game/Tests/Regression/Issue286/Issue286"); }

    virtual bool SetUp() override
    {
        FUnLuaTestBase::SetUp();

        AddLatent([&]()
        {
            lua_getglobal(L, "Result");
            const char* Error = lua_tostring(L, -1);
            RUNNER_TEST_EQUAL(Error, TEXT("passed"));
            return true;
        });

        return true;
    }
};

IMPLEMENT_UNLUA_LATENT_TEST(FUnLuaTest_Issue286, TEXT("UnLua.Regression.Issue286 蓝图 TMap FindRef 错误"))

#endif //WITH_DEV_AUTOMATION_TESTS