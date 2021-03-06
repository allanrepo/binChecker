#include <Unison.h>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <sstream>

using namespace std;

TMResultM onInitTest_WaferID (IntS &LotNumber, StringS &WaferID, IntM &WaferNo, IntS &WaferSlot)
{
    TMResultM val = TM_PASS;
    StringS waferIds;
    FAPROC.Get("Current Equipment:Wafer Scribe", waferIds);
    //waferIds = "b7PRT40760.05B2"; //for debug only
    IntS buffer_leng = waferIds.Length();
    WaferID = waferIds.Substring(1,  buffer_leng-1);
    
    std::stringstream ss;
    ss << waferIds;    
    std::string wd = ss.str();
    
    size_t t = wd.find_first_of('.');
    if (t == std::string::npos)
    {
        std::cout << "ERROR: Didn't find '.' in the wafer scribe" << std::endl;
        val = TM_FAIL;
    }   
   
    ss.str(std::string());
    ss << (wd.substr(t + 1, 2));
    long wafernum;
    ss >> wafernum;    

    StringS casNum, tmpString, str_sublot;
    IntS wafcasnum;
    //casNum = LcdGetGpibConfig("X"); //Wafer Number request : X011   Slot 01/ Cassete 1
    //casNum = "X051";
    str_sublot = casNum.Substring(1,  2);
    wafcasnum = atoi(str_sublot);
    WaferSlot = wafcasnum;
    std::cout << "\nGPIB Wafer Slot = " << wafcasnum << std::endl;
  
    if ((wafernum == 0) || (wafcasnum != wafernum)) {
        std::cout << "Please verify WaferNumber is correct" << std::endl;
        val = TM_FAIL;
    }
    
    std::cout << "Wafer ID: " << wd << "  Wafer Num: " << wafernum << std::endl;     
    
    // forcing all loaded sites (not just active one for this test cycle) to be set
    for (SiteIter siteIter = LoadedSites.Begin(); siteIter.End(); ++siteIter) 
    {
        SITE currSite = siteIter.GetValue();
        WaferNo[currSite] = wafernum;
    }
    
    StringS LotNumberStr = TestProg.GetLotInfo("FabId");
    LotNumber = atoi(LotNumberStr);
  
    return val;
} 


TMResultM onLoadExec(TMResultM val )
{
    return val;
    
    std::cout << "<onLoad> is happening... " << std::endl;
    
    TestProg.SetLotInfo("TestPhase", "ALLAN_TEST");
    TestProg.SetLotInfo("ProtectionCode", "Q");
    
    StringS ss = TestProg.GetLotInfo("TestPhase");
    ss.Print();

    ss = TestProg.GetLotInfo("ProtectionCode");
    ss.Print();
    
    StringM sm;
    
    std::cout << "active " <<   ActiveSites.GetNumSites() << std::endl;
    std::cout << "loaded " <<   LoadedSites.GetNumSites() << std::endl;

    for (SiteIter siteIter = LoadedSites.Begin(); !siteIter.End(); ++siteIter) 
    {
        SITE currSite = siteIter.GetValue();

        std::stringstream ss;            
        ss << "site: " << currSite;
        sm = "hello";//ss.str().c_str();
        StringS st;
        st = "hello";
        st.Print();
    }
    
    IntM mm;
    IntS dd;
    mm = dd;
    
    sm[SITE_1].Print();
    

    
    return val;
}

TMResultM onRestartExec(TMResultM val)
{
    return val;
}

TMResultM onUserExec(TMResultM val, IntS n, FloatS t)
{
    return val;

    std::cout << "<onUser" << n << "> starts... " << std::endl;
    TIME.Wait(t);
    
    std::cout << "<onUser" << n << "> ends after " << t << " seconds" << std::endl;
    return val;
}

TMResultM onWaferStart(TMResultM val)
{
    return val;
    
    StringS waferIds; // = "b7PRT40760.21B2";
    FAPROC.Get("Current Equipment:Wafer Scribe", waferIds);
    
    std::stringstream ss;
    ss << waferIds;    
    std::string wd = ss.str();
    
    size_t t = wd.find_first_of('.');
    if (t == std::string::npos)
    {
        std::cout << "ERROR: Didn't find '.' in the wafer scribe" << std::endl;
        return val;
    }   
   
    ss.str(std::string());
    ss << (wd.substr(t + 1, 2));
    long wafernum;
    ss >> wafernum;    
        
    std::cout << "Wafer ID: " << wd << "  Wafer Num: " << wafernum << std::endl;
               
    return val;

}

bool Unsigned2StringM1D(int n, const UnsignedM1D& um1d, StringM1D& sm1d)
{
    // make sure um1d is at least of size 1 array
    if (!um1d.GetSize()) return false;
    
    // get the multisite unsigned values ofr um1d's first array index
    UnsignedM um = um1d[0];
    
    // declare multisite string variable to store data from um as string
    StringM sm;

    // acquire values for each site of um and copy it into sm    
    for (SiteIter siteIter = ActiveSites.Begin(); !siteIter.End(); ++siteIter) 
    {
        SITE currSite = siteIter.GetValue();
        std::stringstream ss;
        ss << "0x" << std::hex << um[currSite];
        sm[currSite] = ss.str().c_str();            
    }   
    
    // resize sm1d to size n
    sm1d.Clear();
    sm1d.Resize(n, SV_CONTENT_LOSE);

    // let's fill multisite data to sm1d n times
    for (int i = 0; i < n; i++)
    {
        sm1d.SetValue(i,sm);
    }    
    return true;
}

TMResultM onInitTest(TMResultM val)
{
    return val;
    
#if 1
    UnsignedM1D um1d(4);
    for (int i = 0; i < um1d.GetSize(); i++)
    {
        UnsignedM um;
        for (SiteIter siteIter = ActiveSites.Begin(); !siteIter.End(); ++siteIter) 
        {
            SITE currSite = siteIter.GetValue();
            um[currSite] = currSite * 10 + i;
        }
        um.Print();
        um1d.SetValue(i,um);
    }    
#if 1
    StringM1D osm1d;
    Unsigned2StringM1D(5, um1d, osm1d);
    um1d.Print();
    osm1d.Print();
    return val;
#endif    

    StringM1D sm1d(um1d.GetSize());
    for (int i = 0; i < um1d.GetSize(); i++)
    {
        UnsignedM um = um1d[i];
        StringM sm;// = sm1d[i];
        
        for (SiteIter siteIter = ActiveSites.Begin(); !siteIter.End(); ++siteIter) 
        {
            SITE currSite = siteIter.GetValue();
            std::stringstream ss;
            ss << std::hex << um[currSite];
            sm[currSite] = ss.str().c_str();
                    
           // sm1d[i](currSite)        
            //std::cout << "site[" << currSite << "]" << "(" << i << "):" << sm[currSite] << std::endl;
        }
        
        sm1d.SetValue(i,sm);
    }
    um1d.Print();
    sm1d.Print();
       
    
    long hex = 255;
    std::cout << "hex: " << hex << ", " << std::hex << hex << std::endl;

    return val;
#endif  
    
    StringS waferIds = "b7PRT40760.21B2";
   //FAPROC.Get("Current Equipment:Wafer Scribe", waferIds);
    
    std::stringstream ss;
    ss << waferIds;    
    std::string wd = ss.str();
    
    size_t t = wd.find_first_of('.');
    if (t == std::string::npos)
    {
        std::cout << "ERROR: Didn't find '.' in the wafer scribe" << std::endl;
        return val;
    }   
   
    ss.str(std::string());
    ss << (wd.substr(t + 1, 2));
    long wafernum;
    ss >> wafernum;    
        
    std::cout << "Wafer ID: " << wd << "  Wafer Num: " << wafernum << std::endl;

    for (SiteIter siteIter = LoadedSites.Begin(); siteIter.End(); ++siteIter) 
    {
        SITE currSite = siteIter.GetValue();
        std::cout << "site: " << currSite << std::endl;
        
    }

               
    return val;
}

TMResultM ReturnValue(TMResultM val)
{
    return val;
}

TMResultM ResetBinValue_func(UnsignedM &BinResetValue)
{
    //        Expr = Expr { String = "Site(111111->BinSelected,222222->BinSelected,333333->BinSelected,444444->BinSelected,555555->BinSelected,666666->BinSelected,777777->BinSelected,888888->BinSelected,999999->BinSelected,101010->BinSelected,1111111->BinSelected,121212->BinSelected,131313->BinSelected,141414->BinSelected,151515->BinSelected,161616->BinSelected)"; }

    Sites SavedSites = ActiveSites;
    SiteIter site_iter = SavedSites.Begin();

    for (site_iter.Begin(); !site_iter.End(); ++site_iter) {
            BinResetValue[*site_iter] = (int(*site_iter) * 111111);
    }
 
    return TM_PASS;
}

TMResultM SetBinValue_func(UnsignedS MaxHardwareBin, UnsignedM &BinSetValue)
{
    //        Expr = Expr { String = "Site(fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected,fmod(fabs(rand()),MaxHardwareBin)->BinSelected)"; }

    Sites SavedSites = ActiveSites;
    SiteIter site_iter = SavedSites.Begin();

    UnsignedS calculated_bin;
    static unsigned int s = time(NULL);
    for (site_iter.Begin(); !site_iter.End(); ++site_iter) {
        calculated_bin = rand_r(&s);
        calculated_bin = (int)fabs(calculated_bin);
        calculated_bin = (int)fmod(calculated_bin, MaxHardwareBin);
        BinSetValue[*site_iter] = calculated_bin;
    }
 
    return TM_PASS;
}

TMResultM BinCode_test_func(BoolS RandomBinning, UnsignedM BinSelected, UnsignedM ConstantBin, UnsignedS LowLimit, UnsignedS HighLimit, BoolS DisableDatalog)
{
    // This function is used to test the BinCode
    // for Datalog purposes. The Datalog, if enabled,
    // can be compared to the actual BinTool Result.

    TMResultM local_result = TM_PASS;

    UnsignedM test_bin = RandomBinning ? BinSelected : ConstantBin;

    local_result = DLOG.Value(test_bin, LowLimit, HighLimit, 
                   UTL_VOID, UTL_VOID, UTL_VOID, UTL_VOID, UTL_VOID, ER_PASS, !DisableDatalog);

    return local_result;
}

