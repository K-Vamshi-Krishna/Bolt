/***************************************************************************                                                                                     
*   Copyright 2012 Advanced Micro Devices, Inc.                                     
*                                                                                    
*   Licensed under the Apache License, Version 2.0 (the "License");   
*   you may not use this file except in compliance with the License.                 
*   You may obtain a copy of the License at                                          
*                                                                                    
*       http://www.apache.org/licenses/LICENSE-2.0                      
*                                                                                    
*   Unless required by applicable law or agreed to in writing, software              
*   distributed under the License is distributed on an "AS IS" BASIS,              
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.         
*   See the License for the specific language governing permissions and              
*   limitations under the License.                                                   

***************************************************************************/                                                                                     

#pragma OPENCL EXTENSION cl_amd_printf : enable
//#pragma OPENCL EXTENSION cl_khr_fp64 : enable 
namespace bolt{
    namespace cl{
        template<typename T>
        struct greater
        {
            bool operator()(const T &lhs, const T &rhs) const  {return (lhs > rhs);}
        };
        template<typename T>
        struct less 
        {
            bool operator()(const T &lhs, const T &rhs) const  {return (lhs < rhs);}
        };
};
};
//FIXME - this was added to support POD with bolt::cl::greater data types
template<typename T>
struct greater
{
    bool operator()(const T &lhs, const T &rhs) const  {return (lhs > rhs);}
};
template<typename T>
struct less 
{
    bool operator()(const T &lhs, const T &rhs) const  {return (lhs < rhs);}
};



template <typename T_keys, typename T_values, typename Compare>
kernel
void sortByKeyTemplate(global T_keys * keys, 
                 global T_values * values, 
                 const uint stage,
                 const uint passOfStage,
                 global Compare *userComp)
{
    uint threadId = get_global_id(0);
    uint pairDistance = 1 << (stage - passOfStage);
    uint blockWidth   = 2 * pairDistance;
    uint temp;
    uint leftId = (threadId % pairDistance) 
                       + (threadId / pairDistance) * blockWidth;
    bool compareResult;
    values;
    uint rightId = leftId + pairDistance;
    T_keys greater, lesser;
    T_keys leftElement = keys[leftId];
    T_keys rightElement = keys[rightId];
    T_values leftValue = values[leftId];
    T_values rightValue = values[rightId];

    uint sameDirectionBlockWidth = 1 << stage;
    
    if((threadId/sameDirectionBlockWidth) % 2 == 1)
    {
        temp = rightId;
        rightId = leftId;
        leftId = temp;
    }

    compareResult = (*userComp)(leftElement, rightElement);

    if(compareResult)
    {
        keys[rightId] = rightElement;
        keys[leftId]  = leftElement;
        values[rightId] = rightValue;
        values[leftId]  = leftValue;

    }
    else
    {
        keys[rightId] = leftElement;
        keys[leftId]  = rightElement;
        values[rightId] = leftValue;
        values[leftId]  = rightValue;
    }
}
