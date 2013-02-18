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

#define ENABLE_GTEST 1
#define ENABLE_DEBUGGING 0

//#include "common/stdafx.h"
#include <vector>
//#include <array>
#include "bolt/cl/bolt.h"
//#include "bolt/cl/scan.h"
#include "bolt/cl/functional.h"
#include "bolt/cl/reduce_by_key.h"
#include "bolt/unicode.h"
#include "bolt/miniDump.h"

#include <gtest/gtest.h>
//#include <boost/shared_array.hpp>

#include <boost/program_options.hpp>
#include "test_common.h"

#if !ENABLE_GTEST

template<
    typename InputIterator1,
    typename InputIterator2,
    typename OutputIterator1,
    typename OutputIterator2,
    typename BinaryFunction>
std::pair<OutputIterator1, OutputIterator2>
gold_reduce_by_key(
    InputIterator1 keys_first,
    InputIterator1 keys_last,
    InputIterator2 values_first,
    OutputIterator1 keys_output,
    OutputIterator2 values_output,
    BinaryFunction binary_op)
{
    typedef std::iterator_traits< InputIterator1 >::value_type kType;
    typedef std::iterator_traits< InputIterator2 >::value_type vType;
    typedef std::iterator_traits< OutputIterator1 >::value_type koType;
    typedef std::iterator_traits< OutputIterator2 >::value_type voType;
       static_assert( std::is_convertible< vType, voType >::value,
        "InputIterator2 and OutputIterator's value types are not convertible." );

   unsigned int numElements = static_cast< unsigned int >( std::distance( keys_first, keys_last ) );

    // do zeroeth element
    *values_output = *values_first;
    *keys_output = *keys_first;
    unsigned int count = 1;
    // rbk oneth element and beyond

    values_first++;
    for ( InputIterator1 key = (keys_first+1); key != keys_last; key++)
    {
        // load keys
        kType currentKey  = *(key);
        kType previousKey = *(key-1);

        // load value
        voType currentValue = *values_first; 
        voType previousValue = *values_output; //Sure?: Damn sure

        previousValue = *values_output;
        // within segment
        if (currentKey == previousKey)
        {
            //std::cout << "continuing segment" << std::endl;
            voType r = binary_op( previousValue, currentValue);
            *values_output = r;
            *keys_output = currentKey; 
            
        }
        else // new segment
        {
            values_output++;
            keys_output++;            
            *values_output = currentValue;
            *keys_output = currentKey;
            count++; //To count the number of elements in the output array
        }
        values_first++;
    }

    //return result;
    //Print values here: Temporary, things gonna change after pair()
    
    //std::cout<<"Number of ele = "<<count<<std::endl;

    return std::make_pair(keys_output, values_output);


}

int _tmain(int argc, _TCHAR* argv[])
{
 
    int length = 10000;
    std::vector< int > keys( length);
    // keys = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5,...}
    int segmentLength = 0;
    int segmentIndex = 0;
    int key = 1;
    std::vector< int > refInput( length );
    std::vector< int > input( length );
    for (int i = 0; i < length; i++)
    {
        if(std::rand()%3 == 1) key++;
        keys[i] = key;
        refInput[i] = std::rand()%4;
        input[i] = refInput[i];
    }

    // input and output vectors for device and reference
    
    std::vector< int > koutput( length ); 
    std::vector< int > voutput( length ); 
    //std::vector< int > refInput( length );
    std::vector< int > krefOutput( length );
    std::vector< int > vrefOutput( length );
    std::fill(krefOutput.begin(),krefOutput.end(),0);
    std::fill(vrefOutput.begin(),vrefOutput.end(),0);
    // call reduce_by_key

    bolt::cl::equal_to<int> binary_predictor;
    bolt::cl::plus<int> binary_operator;


    bolt::cl::reduce_by_key( keys.begin(), keys.end(), input.begin(), koutput.begin(), voutput.begin(), binary_predictor, binary_operator);
    //for(unsigned int i = 0; i < 256 ; i++)
    //{
    //    std::cout<<"Ikey "<<keys[i]<<" IValues "<<input[i]<<" -> OKeys "<<koutput[i]<<" OValues "<<voutput[i]<<std::endl;
    //}
    gold_reduce_by_key( keys.begin(), keys.end(), refInput.begin(), krefOutput.begin(), vrefOutput.begin(),std::plus<int>());
    
    cmpArrays(krefOutput, koutput);
    cmpArrays(vrefOutput, voutput);

    return 0;
}

#else

template< typename T, typename Iterator >
::testing::AssertionResult cmpArrays2(  std::vector< T >& ref,  std::vector< T >& calc ,   Iterator& ref2,   Iterator& calc2)
{
    for(Iterator i = ref.begin(), j = calc.begin(); i < ref2, j < calc2 ; ++i, ++j )
    {
        EXPECT_EQ( i, j );
    }

    return ::testing::AssertionSuccess( );
}



template<
    typename InputIterator1,
    typename InputIterator2,
    typename OutputIterator1,
    typename OutputIterator2,
    typename BinaryFunction>
std::pair<OutputIterator1, OutputIterator2>
gold_reduce_by_key( InputIterator1 keys_first,
                    InputIterator1 keys_last,
                    InputIterator2 values_first,
                    OutputIterator1 keys_output,
                    OutputIterator2 values_output,
                    BinaryFunction binary_op )
{
    typedef std::iterator_traits< InputIterator1 >::value_type kType;
    typedef std::iterator_traits< InputIterator2 >::value_type vType;
    typedef std::iterator_traits< OutputIterator1 >::value_type koType;
    typedef std::iterator_traits< OutputIterator2 >::value_type voType;
    static_assert( std::is_convertible< vType, voType >::value,
                   "InputIterator2 and OutputIterator's value types are not convertible." );

   unsigned int numElements = static_cast< unsigned int >( std::distance( keys_first, keys_last ) );

    // do zeroeth element
    *values_output = *values_first;
    *keys_output = *keys_first;
    unsigned int count = 1;
    // rbk oneth element and beyond

    values_first++;
    for ( InputIterator1 key = (keys_first+1); key != keys_last; key++)
    {
        // load keys
        kType currentKey  = *(key);
        kType previousKey = *(key-1);

        // load value
        voType currentValue = *values_first; 
        voType previousValue = *values_output; 

        previousValue = *values_output;
        // within segment
        if (currentKey == previousKey)
        {
            voType r = binary_op( previousValue, currentValue);
            *values_output = r;
            *keys_output = currentKey; 
            
        }
        else // new segment
        {
            values_output++;
            keys_output++;            
            *values_output = currentValue;
            *keys_output = currentKey;
            count++; //To count the number of elements in the output array
        }
        values_first++;
    }


    return std::make_pair(keys_output, values_output);
}

TEST(ReduceByKeyBasic, IntegerTest)
{
    int length = 1<<24;
    std::vector< int > keys( length);
    // keys = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5,...}
    int segmentLength = 0;
    int segmentIndex = 0;
    int key = 1;
    std::vector< int > refInput( length );
    std::vector< int > input( length );
    for (int i = 0; i < length; i++)
    {
        if(std::rand()%3 == 1) key++;
        keys[i] = key;
        refInput[i] = std::rand()%4;
        input[i] = refInput[i];
    }

    // input and output vectors for device and reference
    
    std::vector< int > koutput( length ); 
    std::vector< int > voutput( length ); 
    std::vector< int > krefOutput( length );
    std::vector< int > vrefOutput( length );
    std::fill(krefOutput.begin(),krefOutput.end(),0);
    std::fill(vrefOutput.begin(),vrefOutput.end(),0);


    bolt::cl::equal_to<int> binary_predictor;
    bolt::cl::plus<int> binary_operator;


    // call reduce_by_key
    auto p = bolt::cl::reduce_by_key( keys.begin(), keys.end(), input.begin(), koutput.begin(), voutput.begin(), binary_predictor, binary_operator);

#if ENABLE_DEBUGGING

    //for(unsigned int i = 0; i < 256 ; i++)
    //{
    //    std::cout<<"Ikey "<<keys[i]<<" IValues "<<input[i]<<" -> OKeys "<<koutput[i]<<" OValues "<<voutput[i]<<std::endl;
    //}

#endif

    auto refPair = gold_reduce_by_key( keys.begin(), keys.end(), refInput.begin(), krefOutput.begin(), vrefOutput.begin(),std::plus<int>());
    
    //cmpArrays2(krefOutput, koutput, refPair.first, p.first);
    cmpArrays(krefOutput, koutput);
    cmpArrays(vrefOutput, voutput);
   // cmpArrays2(vrefOutput, voutput, refPair.second, p.second);
}



int _tmain(int argc, _TCHAR* argv[])
{
    //  Register our minidump generating logic
    bolt::miniDumpSingleton::enableMiniDumps( );

    //  Initialize googletest; this removes googletest specific flags from command line
    ::testing::InitGoogleTest( &argc, &argv[ 0 ] );

    bool print_clInfo = false;
    cl_uint userPlatform = 0;
    cl_uint userDevice = 0;
    cl_device_type deviceType = CL_DEVICE_TYPE_DEFAULT;

    try
    {
        // Declare supported options below, describe what they do
        boost::program_options::options_description desc( "Scan GoogleTest command line options" );
        desc.add_options()
            ( "help,h",         "produces this help message" )
            ( "queryOpenCL,q",  "Print queryable platform and device info and return" )
            ( "platform,p",     boost::program_options::value< cl_uint >( &userPlatform )->default_value( 0 ),	"Specify the platform under test" )
            ( "device,d",       boost::program_options::value< cl_uint >( &userDevice )->default_value( 0 ),	"Specify the device under test" )
            ;


        boost::program_options::variables_map vm;
        boost::program_options::store( boost::program_options::parse_command_line( argc, argv, desc ), vm );
        boost::program_options::notify( vm );

        if( vm.count( "help" ) )
        {
            //	This needs to be 'cout' as program-options does not support wcout yet
            std::cout << desc << std::endl;
            return 0;
        }

        if( vm.count( "queryOpenCL" ) )
        {
            print_clInfo = true;
        }

        //  The following 3 options are not implemented yet; they are meant to be used with ::clCreateContextFromType()
        if( vm.count( "gpu" ) )
        {
            deviceType	= CL_DEVICE_TYPE_GPU;
        }
        
        if( vm.count( "cpu" ) )
        {
            deviceType	= CL_DEVICE_TYPE_CPU;
        }

        if( vm.count( "all" ) )
        {
            deviceType	= CL_DEVICE_TYPE_ALL;
        }

    }
    catch( std::exception& e )
    {
        std::cout << _T( "Scan GoogleTest error condition reported:" ) << std::endl << e.what() << std::endl;
        return 1;
    }

    //  Query OpenCL for available platforms
    cl_int err = CL_SUCCESS;

    // Platform vector contains all available platforms on system
    std::vector< cl::Platform > platforms;
    //std::cout << "HelloCL!\nGetting Platform Information\n";
    bolt::cl::V_OPENCL( cl::Platform::get( &platforms ), "Platform::get() failed" );

    if( print_clInfo )
    {
        bolt::cl::control::printPlatforms( );
        return 0;
    }

    //  Do stuff with the platforms
    std::vector<cl::Platform>::iterator i;
    if(platforms.size() > 0)
    {
        for(i = platforms.begin(); i != platforms.end(); ++i)
        {
            if(!strcmp((*i).getInfo<CL_PLATFORM_VENDOR>(&err).c_str(), "Advanced Micro Devices, Inc."))
            {
                break;
            }
        }
    }
    bolt::cl::V_OPENCL( err, "Platform::getInfo() failed" );

    // Device info
    std::vector< cl::Device > devices;
    bolt::cl::V_OPENCL( platforms.front( ).getDevices( CL_DEVICE_TYPE_ALL, &devices ), "Platform::getDevices() failed" );

    cl::Context myContext( devices.at( userDevice ) );
    cl::CommandQueue myQueue( myContext, devices.at( userDevice ) );
    bolt::cl::control::getDefault( ).commandQueue( myQueue );

    std::string strDeviceName = bolt::cl::control::getDefault( ).device( ).getInfo< CL_DEVICE_NAME >( &err );
    bolt::cl::V_OPENCL( err, "Device::getInfo< CL_DEVICE_NAME > failed" );

    std::cout << "Device under test : " << strDeviceName << std::endl;

    int retVal = RUN_ALL_TESTS( );

    //  Reflection code to inspect how many tests failed in gTest
    ::testing::UnitTest& unitTest = *::testing::UnitTest::GetInstance( );

    unsigned int failedTests = 0;
    for( int i = 0; i < unitTest.total_test_case_count( ); ++i )
    {
        const ::testing::TestCase& testCase = *unitTest.GetTestCase( i );
        for( int j = 0; j < testCase.total_test_count( ); ++j )
        {
            const ::testing::TestInfo& testInfo = *testCase.GetTestInfo( j );
            if( testInfo.result( )->Failed( ) )
                ++failedTests;
        }
    }

    //  Print helpful message at termination if we detect errors, to help users figure out what to do next
    if( failedTests )
    {
        bolt::tout << _T( "\nFailed tests detected in test pass; please run test again with:" ) << std::endl;
        bolt::tout << _T( "\t--gtest_filter=<XXX> to select a specific failing test of interest" ) << std::endl;
        bolt::tout << _T( "\t--gtest_catch_exceptions=0 to generate minidump of failing test, or" ) << std::endl;
        bolt::tout << _T( "\t--gtest_break_on_failure to debug interactively with debugger" ) << std::endl;
        bolt::tout << _T( "\t    (only on googletest assertion failures, not SEH exceptions)" ) << std::endl;
    }

	return retVal;
}


#endif