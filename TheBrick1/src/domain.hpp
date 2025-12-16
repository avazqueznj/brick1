/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/

void navigateTo(int screenId);
extern String BEARER_TOKEN;
extern void getInternalHeapFreeBytes();


                                // P R O B L E M  ***  D O M A I N 


class userClass{
public:
    String name;
    String username;
    String password;

    userClass(){}

    userClass(String nameParam, String usernameParam, String passwordParam):
        name(nameParam),
        username(usernameParam),
        password(passwordParam){}

    virtual ~userClass(){}

    void clear(){
        name = "";
        username = "";
        password = "";
    }
};


class layoutZoneClass{
public:
    String name;
    String tag;
    String zonePic;
    std::vector<  std::vector<String>  > components;    
    layoutZoneClass(String nameParam, String tagParam, String zonePicParam):
        name(nameParam),tag(tagParam),zonePic(zonePicParam){}
    virtual ~layoutZoneClass(){}
};


class layoutClass{
public:
    String name;
    std::vector<  layoutZoneClass  > zones;    
    layoutClass(String nameParam):name(nameParam){}
    virtual ~layoutClass(){}    
};


class assetClass{
public:
    String ID;
    String layoutName;
    String tag;
    String buttonName;
    assetClass( const String IDparam, const String typeParam, const String tagParam ):
        ID(IDparam),layoutName(typeParam),tag(tagParam) {    
            buttonName += ID;
            buttonName += ": ";
            buttonName += layoutName;
        }

    // Copy constructor
    assetClass(const assetClass& other)
        : ID(other.ID), layoutName(other.layoutName), tag(other.tag), buttonName(other.buttonName) {
    }
            
    virtual ~assetClass(){}            
};


class inspectionTypeClass{
public:
    String name;
    std::vector<  String  > layouts;    
    std::vector<  std::vector<String>  > formFields;    
    inspectionTypeClass(String nameParam):name(nameParam){}
    virtual ~inspectionTypeClass(){}    
};


// ***

class defectClass {
public:
    assetClass asset;
    String zoneName;
    String componentName;
    String defectType;
    int severity;
    String notes;
    String time;

    defectClass(
        assetClass assetParam,
        const String& zoneNameParam,
        const String& componentNameParam,
        const String& defectTypeParam,
        int severityParam,
        const String& notesParam,
        const String& timeParam)
    : asset(assetParam),
      zoneName(zoneNameParam),
      componentName(componentNameParam),
      defectType(defectTypeParam),
      severity(severityParam),
      notes(notesParam),
      time(timeParam)
    {}

    bool isSameComponent(const defectClass& other) const {
        bool sameAsset = (asset.ID == other.asset.ID);
        bool sameZone = (zoneName == other.zoneName);
        bool sameComponent = (componentName == other.componentName);
        bool result = sameAsset && sameZone && sameComponent;
        return result;
    }    
};


class inspectionClass {
public:

    String id;
    String company;  

    inspectionTypeClass* type = NULL;
    std::vector<assetClass> assets;    
    std::vector<defectClass> defects;
    std::vector<  String  > inspectionFormFieldValues; 

    String startTime;
    String offset;
    String dst;

    String driver_username;    
    String driver_name;

private:

    String finishedTimeString = "";
    DateTime finishedTime;

//============================================================================

public:

    inspectionClass(){
        id = newUUID();
    }

    void clear() {
        id = newUUID();
        company = "";

        type = NULL;
        assets.clear();
        defects.clear();
        inspectionFormFieldValues.clear();
        startTime = "";
        offset = "";
        dst = "";

        driver_username = "";        
        driver_name = "";
    }

    void finished( int timeOffsetFromUTC ){     
        
        // set the clock
        finishedTime =  rtc->now();  

        // set the strings for humans
        char timeString[30];
        DateTime local = finishedTime + TimeSpan(timeOffsetFromUTC * 60);
        snprintf(timeString, sizeof(timeString), "%04d-%02d-%02d %02d:%02d:%02d",
                local.year(), local.month(), local.day(), local.hour(), local.minute(), local.second());        
        finishedTimeString = timeString;

    }

    String toHumanString() const {
        String result = "INSPECTION\n";

        result += "ID: " + id + "\n";
        result += "Company: " + company + "\n";

        result += "Driver: ";
        result += driver_username;
        result += " ";       
        result += driver_name;
        result += "\n";            

        result += "Start time: ";
        result += startTime;
        result += "\n";           

        result += "finish time: ";
        result += finishedTimeString;
        result += "\n";           
        

        // --- Inspection Type ---
        if (type != NULL) {
            result += "Type: ";
            result += type->name;
            result += "\n";

            // result += "Layouts:\n";
            // for (const auto& layout : type->layouts) {
            //     result += " - ";
            //     result += layout;
            //     result += "\n";
            // }

            result += "Form Fields:\n";
            size_t rowIndex = 0;
            for (const auto& row : type->formFields) {
                result += "";
                result += String(rowIndex);
                result += ": ";
                result += row[0];
                                        
                // Add the value if it exists
                if (rowIndex < inspectionFormFieldValues.size()) {
                    result += " : ";
                    result += inspectionFormFieldValues[rowIndex];
                } else {
                    result += " : <unset>";
                }
                result += "\n";
                ++rowIndex;
            }

        } else {
            result += "Type: NULL\n";
        }      

        // --- Assets ---
        result += "Assets:\n";
        for (const auto& asset : assets) {
            result += " - ID: ";
            result += asset.ID;
            result += ", Layout: ";
            result += asset.layoutName;
            result += ", Tag: ";
            result += asset.tag;
            result += "\n";
        }

        // --- Defects: severity == 0 first ---
        // result += "Defects (sev == 0):\n";
        // for (const auto& defect : defects) {
        //     if (defect.severity == 0) {
        //         result += " - Asset ID: ";
        //         result += defect.asset.ID;
        //         result += ", Zone: " + defect.zoneName;
        //         result += ", Component: " + defect.componentName;
        //         result += ", Type: " + defect.defectType;
        //         result += ", Severity: " + String(defect.severity);
        //         result += ", Notes: " + defect.notes;
        //         result += "\n";
        //     }
        // }

        // --- Defects: severity > 0 after ---
        result += "Defects:\n";
        for (const auto& defect : defects) {
            if (defect.severity > 0) {
                result += "- Asset: ";                
                result += defect.asset.ID;
                result += " Zone: " + defect.zoneName;
                result += ", " + defect.componentName;
                result += ": " + defect.defectType;
                result += " (" + String(defect.severity);
                result += ") Notes: " + defect.notes;
                result += "\n\n";
            }
        }

        return result;
    }   
    
    //====================================
    //============
    //====================================

    static String sanitizeEDIValue(const String &in) {
        String s = in;  // make a copy so we don't mutate the original        
        s.replace('*', '_');      
        s.replace('\n', ' ');
        s.replace('\r', ' ');
        return s;
    }

    String toEDI() const {

        String result = "BRICKINSPECTION*1\n";

        // display header 
        result += 
            "DISPLAYHEADER*" 
            + String( finishedTime.unixtime() ) +
            + "*" + id.substring(id.length() - 5) + "(" + BEARER_TOKEN.substring(BEARER_TOKEN.length() - 5) + ")"
            + "*" + finishedTimeString 
            + "*" + driver_name 
            + "*" + assets[0].buttonName
            + "*\uF071"
            + "\n";

        result += "INSPHEADER\n";

        result += "INSPID*" + id + "\n";
        result += "COMPANYID*" + company + "\n";

        result += "DRIVER*";
        result += driver_name;
        result += "*";       
        result += driver_username;
        result += "\n";
            
        result += "INSPSTARTTIME*";
        result += startTime;
        result += "\n";           

        result += "INSPSUBTIME*";
        result += finishedTimeString;
        result += "\n";           

        result += "INSPTIMEOFFSET*";
        result += offset;
        result += "\n";           

        result += "INSPTIMEDST*";
        result += dst;
        result += "\n";                 

        // --- Assets ---
        result += "ASSETS\n";
        for (const auto& asset : assets) {
            result  += "ASSET*" + asset.ID             
            + "*" + asset.layoutName           
            + "*" + asset.tag
            + "\n";
        }

        // --- Inspection Type ---
        result += "INSPTYPE\n";

        result += "INSPTYPENAME*" + type->name +"\n";

        result += "FORMFIELDS\n";

        size_t rowIndex = 0;
        for (const auto& row : type->formFields) {

            // name
            result += "FF*" + row[0];
        
            // value
            if (rowIndex < inspectionFormFieldValues.size()) {
                result += "*";
                result += sanitizeEDIValue(inspectionFormFieldValues[rowIndex]);
            } else {
                result += "*NULL";
            }

            result += "\n";
            ++rowIndex;
        }

        // --- Defects: severity == 0 first ---
        result += "CHECKS\n";
        for (const auto& defect : defects) {
            if (defect.severity == 0) {
                result += "CHECK*";
                result += defect.asset.ID;
                result += "*" + defect.zoneName;
                result += "*" + defect.componentName;
                result += "*" + defect.defectType;
                result += "*" + String(defect.severity);
                result += "*" + defect.time;
                result += "*" + sanitizeEDIValue(defect.notes);
                result += "\n";
            }
        }

        // --- Defects: severity > 0 after ---
        result += "DEFECTS\n";
        for (const auto& defect : defects) {
            if (defect.severity > 0) {
                result += "DEFECT*";                
                result += defect.asset.ID;
                result += "*" + defect.zoneName;
                result += "*" + defect.componentName;
                result += "*" + defect.defectType;
                result += "*" + String(defect.severity);
                result += "*" + defect.time;
                result += "*" + sanitizeEDIValue(defect.notes);
                result += "\n";
            }
        }

        result += "END***\n";

        return result;
    }   

};




//-------------------------------------------------

//    D O M A I N   M A N A G E R 

//-------------------------------------------------

#include <stdio.h>   
#define INSP_SUBMIT_ERROR  "\uF071"
#define INSP_SUBMIT_OK     "\uF00C"
extern "C" void* sdram_malloc(size_t size);
extern "C" void  sdram_free(void* ptr);
template <typename T>
struct SDRAMAllocator {
    using value_type = T;

    SDRAMAllocator() noexcept {}
    template <class U> SDRAMAllocator(const SDRAMAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        void* p = sdram_malloc(n * sizeof(T));
        if (!p) {
            throw std::runtime_error("Vector SDRAM allocator failed!!!");
        }
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) noexcept {
        sdram_free(p);
    }
};
extern QSPIFBlockDevice qspi;
extern mbed::FATFileSystem fs;


class domainManagerClass {
public:

    // from the config
    String company = ""; 
    String serverURL = "10.0.0.32"; 

    int  timeZoneIndex = 0;    
    int  timeOffsetFromUTC = 0;
    int DST = 0;

    // internal
    String getConfigPath = "NOT SET";
    String postInspectionsPath = "NOT SET";

private:

    // database
    std::vector<userClass, SDRAMAllocator<userClass>> users;
    std::vector<assetClass, SDRAMAllocator<assetClass>> assets;
    std::vector<layoutClass, SDRAMAllocator<layoutClass>> layouts;
    std::vector<inspectionTypeClass, SDRAMAllocator<inspectionTypeClass>> inspectionTypes;

//----------------------------

public:


    // Const getters
    const std::vector<userClass, SDRAMAllocator<userClass>>*
    getUsers() const { return &users; }

    const std::vector<assetClass, SDRAMAllocator<assetClass>>*
    getAssets() const { return &assets; }

    const std::vector<layoutClass, SDRAMAllocator<layoutClass>>*
    getLayouts() const { return &layouts; }

    const std::vector<inspectionTypeClass, SDRAMAllocator<inspectionTypeClass>>*
    getInspectionTypes() const { return &inspectionTypes; }

    // has
    bool isLoaded = false;
    commsClass* comms = NULL;



    // inspection
    inspectionClass currentInspection;

    //logged user
    userClass loggedUser;

    // singleton
    static domainManagerClass* getInstance() {
        static domainManagerClass instance;  // Guaranteed to be created once (thread-safe in C++11+)
        return &instance;
    }    

    domainManagerClass(){        
        comms = new commsClass();            
    }

    virtual ~domainManagerClass(){        
        delete comms;
    }

    void emptyAll() {

        currentInspection.clear();

        assets.clear();
        layouts.clear();
        inspectionTypes.clear();
        users.clear();
    }

    void logout(){

        loggedUser.clear();
        currentInspection.clear();

        navigateTo( SCREEN_ID_LOGIN_SCREEN );
    }


    bool login(String usernameParam, String passwordParam) {


        Serial.print("Login ....");

        if( users.size() == 0 ){
            throw std::runtime_error("No users loaded, cannot login");
        }

        for (size_t i = 0; i < users.size(); i++) {
            if (users[i].username == usernameParam && users[i].password == passwordParam) {

                loggedUser = users[i];

                Serial.print("Login successful: ");
                Serial.println(users[i].name);
                
                return true;
            }
        }

        Serial.print("Login .... INVALID");
        return false;
    }

    String sync(){
        
        spinnerStart();

        try{

                getInternalHeapFreeBytes();

                // get config
                comms->connectToWifi();

                {                    
                    std::vector<String> config;
                    getInternalHeapFreeBytes();
                    comms->GET( serverURL , getConfigPath + "?company=" + domainManagerClass::getInstance()->company, config );
                    getInternalHeapFreeBytes();
                    parse( &config );
                    saveTextVecToQSPI( "/qspi/brickconfig.txt" , &config  );
                    getInternalHeapFreeBytes();
                }
                    
                // sync inspes
                getInternalHeapFreeBytes();
                int sentInspections = retryAllPendingInspections();
                
                getInternalHeapFreeBytes();
                int loadedPics = 0;
                try {
                    Serial.println("syncPics .....");                                        
                    loadedPics = syncPics();
                    WiFi.end();                    
                } catch (const std::runtime_error& e) {
                    Serial.println("[WARN] syncPics failed, continuing without pics.");
                    Serial.println(e.what());
                    WiFi.end();
                }



                    String syncMessage = "Sync successful. \n";

                    syncMessage += domainManagerClass::getInstance()->assets.size();
                    syncMessage += " assets, ";

                    syncMessage += domainManagerClass::getInstance()->layouts.size();
                    syncMessage += " layouts, ";

                    syncMessage += domainManagerClass::getInstance()->inspectionTypes.size();
                    syncMessage += " types, ";

                    syncMessage += domainManagerClass::getInstance()->users.size();
                    syncMessage += " users, ";       

                    syncMessage += sentInspections;
                    syncMessage += " pending inspections, ";       

                    syncMessage += loadedPics;
                    syncMessage += " pictures. ";       

                    spinnerEnd();                           

                    return syncMessage;
                                

        }catch( const std::runtime_error& error ){
            spinnerEnd();
            String chainedError = String( "ERROR: Could not sync: " ) + error.what();            
            throw std::runtime_error( chainedError.c_str() );
        }


    }


    void parse( std::vector<String>* config ){     

        Serial.println( "Parsing ..." );

        std::vector<String>::iterator iterator = config->begin();    
        while ( iterator != config->end() ) {   

            // HEADER ----->
            if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");        

            std::vector<String> tokens = tokenize( *iterator , '*' );     
            if( tokens[ 0 ] == "BRICKCONFIG" ){ 
                Serial.println( "[BRICKCONFIG] found..." );

                emptyAll();

                // ASSETS ----->
                if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");
                tokens = tokenize( *iterator , '*' );
                if( tokens[ 0 ] == "ASSETS" ){ 
                    Serial.println( "[ASSETS] found... load assets!" );

                    while( true ){ // read the next asset
                        ++iterator;tokens = tokenize( *iterator , '*' );
                        if( tokens[ 0 ] != "AS" ) break;

                        Serial.println( "[ASSET] found... load asset!" );
                        if( tokens.size() != 4 ) throw std::runtime_error( "Parse AS error, expecting 4 tokens" );
                        assets.push_back( assetClass( tokens[ 1 ] , tokens[ 2 ] , tokens[ 3 ]  ) );        
                        Serial.println( "Asset added!!" );
                    }

                }else{
                    throw std::runtime_error( "Parse error, expecting ASSETS" );
                }                    

                //  LAYOUTS ----->
                if( tokens[ 0 ] == "LAYOUTS" ){ 
                    Serial.println( "[LAYOUTS] found... load layout!" );
                    
                    if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");
                    tokens = tokenize( *iterator , '*' );                                        
                    while( true ){  // get the next  layout                        
                        if( tokens[ 0 ] != "LAY" ) break; // end layouts?                        

                        // get name
                        String layoutName; Serial.println( "Found LAY ..." );                        
                        if( tokens.size() == 2 ){
                            layoutName = tokens[ 1 ];
                        }else throw std::runtime_error( "Parse error token LAY expecting 2 tokens " );

                        // make layout step 1
                        layoutClass layout(layoutName);

                        // ZONES --->
                        if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");
                        tokens = tokenize( *iterator , '*' );
                        while( true ){ // read the next zone                            
                            if( tokens[ 0 ] != "LAYZONE" ) break;

                            Serial.println( "Found LayoutZone ..." );                        
                            if( tokens.size() == 4 ){
                                String zoneTag = tokens[ 1 ];
                                String zoneName = tokens[ 2 ];
                                String zonePic = tokens[ 3 ];

                                // make layout step 2
                                layoutZoneClass zone(zoneName,zoneTag,zonePic);

                                // COMPONENTS ----->
                                while( true ){ // read the next component
                                    if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");
                                    tokens = tokenize( *iterator , '*' );
                                    if( tokens[ 0 ] != "ZONECOMP" ) break;

                                    Serial.println( "Found Zone component & defects..." );                        
                                    if( tokens.size() >= 3 ){

                                        // make layout step 3
                                        zone.components.push_back( tokens );

                                    }else throw std::runtime_error( "Parse error token ZONECOMP expecting >=3 tokens " );                                                                        
                                }

                                layout.zones.push_back( zone );

                            }else throw std::runtime_error( "Parse error token LAYZONE expecting 4 tokens " );
                        }

                        // add layout
                        layouts.push_back( layout );

                    }                 
                }else{
                    throw std::runtime_error( "Parse error, expecting LAYOUTS" );
                }     

                //  INSPE TYPES ----->
                if( tokens[ 0 ] == "INSPTYPES" ){ 
                    Serial.println( "[INSPTYPES] found... load inpection types!" );

                    if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");
                    tokens = tokenize( *iterator , '*' );                                        
                    while( true ){  // get the next inspt
                        if( tokens[ 0 ] != "INSP" ) break; // end inspts
                        Serial.println( "Found INSP ..." );                        

                        // get name
                        String inspName; 
                        if( tokens.size() >= 3 ){
                            inspName = tokens[ 1 ];                            
                        }else throw std::runtime_error( "Parse error token INSP expecting >= 3 tokens " );

                        inspectionTypeClass inspType(inspName);
                        tokens.erase(tokens.begin(), tokens.begin() + 2);
                        inspType.layouts = tokens;

                        // FORM FIELDS --->                    
                        while( true ){ // read the next ff    
                            if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");
                            tokens = tokenize( *iterator , '*' );                        
                            if( tokens[ 0 ] != "INSPFF" ) break;

                            Serial.println( "Found FF ..." );                        
                            if( tokens.size() == 4 ){

                                tokens.erase(tokens.begin(), tokens.begin() + 1);
                                inspType.formFields.push_back( tokens );

                            }else throw std::runtime_error( "Parse error token FF expecting 4 tokens " );

                        }

                        // add inspt
                        inspectionTypes.push_back( inspType );

                    }     

                }else{
                    throw std::runtime_error( "Parse error, expecting INSPTYPES" );
                }                    

                //--

                //  USERS TYPES ----->
                if( tokens[ 0 ] == "USERS" ){ 
                    Serial.println( "[USERS] found... " );
                    
                    while( true ){  // get the next user            
                        if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");
                        tokens = tokenize( *iterator , '*' );                                        
                        if( tokens[ 0 ] != "USER" ) break; // end users
                        Serial.println( "Found USER ..." );                        

                        // get data
                        String name; 
                        String username; 
                        String password; 
                        if( tokens.size() == 4 ){
                            name = tokens[ 1 ];
                            username = tokens[ 2 ];
                            password = tokens[ 3 ];
                        }else throw std::runtime_error( "Parse error token USER expecting 4 tokens " );

                        userClass nextUser( name, username, password );
                        users.push_back( nextUser );
                    }     

                }else{
                    throw std::runtime_error( "Parse error, expecting USERS" );
                }                    


                //--

                // done?
                if( tokens[ 0 ] == "END" ){ 
                    Serial.println( "Found [END]" );                
                    isLoaded = true;
                    printDebugContents();
                    return;
                }else{
                    Serial.println( "Parse error un expected token" );                
                    Serial.println( tokens[ 0 ] );                                    
                    throw std::runtime_error( "Parse error un expected token" );                    
                }

            }
        } // scanning

        throw std::runtime_error( "Parse error: unexpected end of file." );   
    }
    

    void printDebugContents() {
        Serial.println("========= ASSETS =========");
        for (const assetClass& a : assets) {
            Serial.print("ID: "); Serial.print(a.ID);
            Serial.print(", Layout: "); Serial.print(a.layoutName);
            Serial.print(", Tag: "); Serial.println(a.tag);
        }

        Serial.println("========= LAYOUTS =========");
        for (const layoutClass& l : layouts) {
            Serial.print("Layout Name: "); Serial.println(l.name);
            for (const layoutZoneClass& z : l.zones) {
                Serial.print("  Zone Name: "); Serial.print(z.name);
                Serial.print(", Tag: "); Serial.println(z.tag);
                Serial.print(", Pic: "); Serial.println(z.zonePic);
                for (const auto& compGroup : z.components) {
                    Serial.print("    Component Group: ");
                    for (const String& comp : compGroup) {
                        Serial.print(comp); Serial.print(" | ");
                    }
                    Serial.println();
                }
            }
        }

        Serial.println("========= INSPECTION TYPES =========");
        for (const inspectionTypeClass& i : inspectionTypes) {
            Serial.print("Inspection Name: "); Serial.println(i.name);
            
            Serial.print("  Layouts: ");
            for (const String& layout : i.layouts) {
                Serial.print(layout); Serial.print(" | ");
            }
            Serial.println();

            Serial.println("  Form Fields:");
            for (const auto& fieldGroup : i.formFields) {
                Serial.print("    ");
                for (const String& field : fieldGroup) {
                    Serial.print(field); Serial.print(" | ");
                }
                Serial.println();
            }
        }

        Serial.println("========= USERS =========");
        for (size_t i = 0; i < users.size(); i++) {
            Serial.print("User ");
            Serial.print(i);
            Serial.print(": Name=");
            Serial.print(users[i].name);
            Serial.print(", Username=");
            Serial.print(users[i].username);
            Serial.print(", Password=");
            Serial.println( "*" );  // users[i].password
        }
        Serial.println("-------------------");

    }

    //==========================================================================================================================================
    //==========================================================================================================================================
    //==========================================================================================================================================    


    void doSubmitInspection( String EDI, String inspectionText){

        // for the record                        
        Serial.println( EDI );                        
        String filingRecord = EDI + "\n" + inspectionText + "\n";
        String path = saveInspectionToDisk( filingRecord );
        String result = "";              

        try{
            result =  comms->POST( serverURL, postInspectionsPath + "?company=" + company,  EDI );            
            updateInspectionFileStatus( path, INSP_SUBMIT_OK ,result, EDI, inspectionText );

            retryAllPendingInspections();

        }catch( const std::runtime_error& error ){

            updateInspectionFileStatus( path, INSP_SUBMIT_ERROR ,error.what(), EDI, inspectionText );
            String errorMessage = String( "ERROR: Could not submit: " )  + error.what();
            throw std::runtime_error( errorMessage.c_str() );
        }

    }

    void doReSubmitInspection( String path, String EDI, String inspectionText){

        // for the record                        
        Serial.println( EDI );                    
        String filingRecord = EDI + "\n" + inspectionText + "\n";
        String result = "";  
        
        try{
            result =  comms->POST( serverURL, postInspectionsPath + "?company=" + company,  EDI );
            updateInspectionFileStatus( path, INSP_SUBMIT_OK ,result, EDI, inspectionText );

        }catch( const std::runtime_error& error ){
            
            updateInspectionFileStatus( path, INSP_SUBMIT_ERROR ,error.what(), EDI, inspectionText );
            String errorMessage = String( "ERROR: Could not submit: " )  + error.what();
            throw std::runtime_error( errorMessage.c_str() );
        }
    }    

    void doSaveInspection(){

        // save it
        String filingRecord = 
            currentInspection.toEDI()  + "\n" +
            currentInspection.toHumanString() + "\n";
        saveInspectionToDisk( filingRecord );                       
    }


    String saveInspectionToDisk(const String& inspectionFilingRecord) {

        // Slot selection logic as above (find empty or oldest based on parsed DISPLAYHEADER* timestamp)

        int oldestSlot = 1;  // the one to delete
        uint32_t oldestTime = UINT32_MAX;  //oldest to start
        Serial.println( "SAVE: find slot...." );
        for (int i = 1; i <= NUM_INSPECTION_SLOTS; ++i) {

            String path = "/kv/insp" + String(i);
            uint32_t ts = 0;
            try {

                // load
                std::vector<String> file = loadFromKVStore(path);
                // parse
                for (const String& line : file) {
                    if (line.startsWith("DISPLAYHEADER*")) {
                        std::vector<String> tokens = tokenize( line, '*' );
                        ts = (uint32_t)( tokens[1] ).toInt();
                        break;
                    }
                }
            } catch (...) {
                Serial.println( "ERROR!!!: Cannor parse slot ovewrite it!" );
                ts = 0;
                oldestSlot = i;
                break;

            }

            // checking time stamp
            if (ts == 0) {
                oldestSlot = i;
                break;
            }
            if (ts < oldestTime) {
                oldestTime = ts;
                oldestSlot = i;
            }
        }

        // DO SAVE
        String path = "/kv/insp" + String(oldestSlot);
        Serial.println( "SAVE: saving: " + path );
        saveToKVStore(path, inspectionFilingRecord);
        Serial.print("Saved inspection to slot ");
        Serial.println(oldestSlot);

        return( path );
    }

    void updateInspectionFileStatus( String path, String statusCode, String serverReply, String EDI, String inspectionText  ){

        // DO SAVE
        EDI.replace(INSP_SUBMIT_ERROR, statusCode); // not fouond , maybe resending that is ok

        Serial.println( "UPDATE!!: " + path );
        saveToKVStore(
            path, 
            EDI + "\n" + inspectionText + "\n"
            + serverReply + "\n"
        );
        Serial.print("UPDATED! inspection to slot ");
        Serial.println(path);

    }

    int retryAllPendingInspections() {

        Serial.println( "RETRY PENDING ====================================================" );

        // check slots
        int inspectionsSent = 0;
        for (int i = 1; i <= NUM_INSPECTION_SLOTS; ++i) {

            String path = "/kv/insp" + String(i);
            std::vector<String> file;
            try {
                file = loadFromKVStore(path);
            } catch (...) {
                continue; // skip unreadable slots
            }
            if (file.empty()) continue;

            // Find the DISPLAYHEADER line
            String displayHeader;
            for (size_t j = 0; j < file.size(); ++j) {
                if (file[j].startsWith("DISPLAYHEADER*")) {
                    displayHeader = file[j];
                    break;
                }
            }
            if (displayHeader.length() == 0) continue; // no header? skip

            // Find last star and last field (status)
            std::vector<String> fields = tokenize(displayHeader, '*');
            if (fields.empty()) continue; // Or maybe log bad header
            if (fields.size() <= 6) continue; 
            String status = fields[6];
            status.trim();

            // Check if not submitted (pending)
            if (status != INSP_SUBMIT_OK) {

                String EDI, inspectionText;
                bool inInspection = true;
                for (size_t j = 0; j < file.size(); ++j) {
                    if (inInspection) {
                        EDI += file[j] + "\n";
                        if (file[j].startsWith("END")) inInspection = false;
                    } else {
                        inspectionText += file[j] + "\n";
                    }
                }
                // Remove trailing newlines if needed
                EDI.trim();
                inspectionText.trim();

                // Try to resubmit, ignore errors 
                try {

                    Serial.println( "RETRY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
                    doReSubmitInspection(path, EDI, inspectionText);
                    inspectionsSent++;
                    Serial.println( "RETRY >>>>>>>>>>>>>>> SUCCESS!!!" );

                }catch( const std::runtime_error& error ){                    
                    
                    Serial.println( "RETRY >>>>>>>>>>>>>>> FAIL!!!" );                    
                    // keep going , eat it
                }
            } 

        } // for

        return inspectionsSent;
    }    
    
    //==========================================================================================================================================
    //==========================================================================================================================================
    //==========================================================================================================================================    

    uint8_t* downloadImageToSDRAM(
        WiFiSSLClient& serverConnection1,
        const String& uuid,
        size_t& outLen
    ) {
        outLen = 0;
        String path = "/api/device/images/" + uuid;

        Serial.println("==============================");
        Serial.println("[IMG] Starting image download (allocate-once)");
        Serial.print("[IMG] Server: "); Serial.println(domainManagerClass::getInstance()->serverURL);
        Serial.print("[IMG] Path: "); Serial.println(path);
        Serial.println("==============================");
    
        uint8_t* buffer = 0;
        int httpStatus = 0;
        String statusLine;
        try{
            Serial.println("[IMG] Sending HTTP request...");
            serverConnection1.print("GET " + path + " HTTP/1.1\r\n");
            serverConnection1.print("Host: " + domainManagerClass::getInstance()->serverURL + "\r\n");
            serverConnection1.print("Authorization: Bearer " + BEARER_TOKEN + "\r\n");
            serverConnection1.print("Accept: */*\r\n");
            serverConnection1.print("Connection: close\r\n");
            serverConnection1.print("\r\n");

            int contentLength = -1;
            String line;
            String contentType = "";
            bool isStatusLine = true;

            Serial.println("[IMG] Reading HTTP headers...");

            int headerTries = 0;
            const int HEADER_MAX_TRIES = 100; // 100 * 100ms = 10s

            Serial.println("[IMG] Reading HTTP headers...");
            while (serverConnection1.connected()) {
                if (!serverConnection1.available()) {
                    Serial.println("wait .... "); 
                    delay(100);
                    headerTries++;
                    if (headerTries >= HEADER_MAX_TRIES) {                        
                        Serial.println("[FATAL] HTTP header read timeout!");
                        throw std::runtime_error("HTTP header read timeout");
                    }
                    continue;
                }
                headerTries = 0; // Reset on progress
                line = serverConnection1.readStringUntil('\n');
                line.trim();
                if (line.length() == 0) break;

                Serial.print("[HDR] "); Serial.println(line);

                if (isStatusLine) {
                    statusLine = line;
                    isStatusLine = false;
                    int firstSpace = line.indexOf(' ');
                    int secondSpace = line.indexOf(' ', firstSpace + 1);
                    if (firstSpace > 0 && secondSpace > firstSpace) {
                        httpStatus = line.substring(firstSpace + 1, secondSpace).toInt();
                        Serial.print("[HDR] HTTP Status = "); Serial.println(httpStatus);
                    }
                }
                if (line.startsWith("Content-Length:")) {
                    contentLength = line.substring(15).toInt();
                    Serial.print("[HDR] Content-Length = ");
                    Serial.println(contentLength);
                }
                if (line.startsWith("Content-Type:")) {
                    contentType = line.substring(13);
                    contentType.trim();
                    Serial.print("[HDR] Content-Type = ");
                    Serial.println(contentType);
                }
                if (line.startsWith("Transfer-Encoding:") && line.indexOf("chunked") >= 0) {
                    Serial.println("[FATAL] Chunked transfer not allowed!");
                    throw std::runtime_error("Image download error: chunked transfer not supported");
                }
            }

            // --- Check HTTP Status ---
            if (httpStatus < 200 || httpStatus >= 300) {
                // Not OK! Read and log the body as error message
                Serial.print("[FATAL] HTTP error code: "); Serial.println(httpStatus);
                String errorMsg;
                while (serverConnection1.available()) {
                    char c = serverConnection1.read();
                    errorMsg += c;
                }
                Serial.print("[FATAL] Server error message: ");
                Serial.println(errorMsg);
                String error = "Image download failed with HTTP status " +  String(httpStatus) + errorMsg;
                throw std::runtime_error(
                    error.c_str()
                );
            }

            if (!contentType.startsWith("image/jpeg")) {
                Serial.print("[FATAL] Bad Content-Type: "); Serial.println(contentType);
                throw std::runtime_error("Image download error: not a JPEG (Content-Type mismatch)");
            }

            if (contentLength <= 0) {
                Serial.println("[FATAL] No Content-Length!");
                throw std::runtime_error("Image download error: missing Content-Length");
            }

            Serial.print("[IMG] Allocating SDRAM: ");
            Serial.println(contentLength);
            buffer = (uint8_t*)SDRAM.malloc(contentLength);
            if (!buffer) {
                Serial.println("[FATAL] SDRAM alloc failed!");
                throw std::runtime_error("SDRAM allocation failed");
            }

            size_t totalRead = 0;
            size_t tries = 0;
            const size_t MAX_TRIES = 100; 
            while (totalRead < (size_t)contentLength) {
                int n = serverConnection1.read(buffer + totalRead, contentLength - totalRead);
                if (n > 0) {
                    totalRead += n;
                    Serial.print("[IMG] Bytes read: "); Serial.println(totalRead);
                    tries = 0;
                } else if (!serverConnection1.connected()) {
                    Serial.println("[FATAL] Connection closed before download complete.");
                    break;
                } else {
                    delay(100);
                    Serial.print("[IMG] No data, wait .... ");        
                    tries++;
                    if (tries >= MAX_TRIES) {
                        Serial.println("[FATAL] Read timeout: no progress for 10 seconds.");
                        throw std::runtime_error("Image download failed: network timeout (no progress for 10 seconds)");
                        break;
                    }
                }
            }


            if (totalRead != (size_t)contentLength) {
                SDRAM.free(buffer);
                Serial.print("[FATAL] Only read "); Serial.print(totalRead); Serial.print(" of "); Serial.println(contentLength);
                throw std::runtime_error("Image download incomplete (Content-Length mismatch)");
            }

            outLen = totalRead;
            Serial.println("[IMG] Download complete");
            Serial.print("[IMG] Total bytes: "); Serial.println(outLen);
            Serial.println("==============================");

            return buffer;

        } catch(...) {
            if (buffer) {
                SDRAM.free(buffer);
                buffer = nullptr;
            }        
            throw;
        }
    }    


    //=========================================================
    //=========================================================


    int syncPics(  ) {

        Serial.println("SYNC PICS ====================================================");

        int picsLoaded  = 0;
        int picsDeleted = 0;

        openQSPI();

        // 2) Build list of expected picture filenames (NO path, just "brickimg_<uuid>.jpg")
        std::vector<String> expectedPics;
        for (size_t i = 0; i < layouts.size(); i++) {

            layoutClass& layout = layouts[i];
            for (size_t j = 0; j < layout.zones.size(); j++) {

                layoutZoneClass& zone = layout.zones[j];
                if (zone.zonePic.length() == 0) {
                    continue;
                }

                String fname = "brickimg_";
                fname += zone.zonePic;
                fname += ".jpg";
                bool alreadyThere = false;
                for (size_t k = 0; k < expectedPics.size(); k++) {
                    if (expectedPics[k] == fname) {
                        alreadyThere = true;
                        break;
                    }
                }
                if (!alreadyThere) {
                    expectedPics.push_back(fname);
                }
            }
        }

        // 3) Remove orphaned brickimg_* files in /qspi/ that are NOT in expectedPics
        DIR* dir = opendir("/qspi/");
        if (dir == NULL) {
            Serial.println("[PICS] Failed to open /qspi/ directory.");
        } else {

            struct dirent* de;
            while ((de = readdir(dir)) != NULL) {

                const char* name = de->d_name;
                if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'))) {
                    continue;
                }
                if (de->d_type == DT_DIR) {
                    continue;
                }

                // we only care about "brickimg_..."
                const char* prefix = "brickimg_";
                int prefixLen = 9; // strlen("brickimg_");
                bool hasPrefix = true;
                for (int i = 0; i < prefixLen; i++) {
                    if (name[i] != prefix[i]) {
                        hasPrefix = false;
                        break;
                    }
                }
                if (!hasPrefix) {
                    continue;   // leave wifi files alone
                }

                // check if this file is in expectedPics
                bool keep = false;
                for (size_t k = 0; k < expectedPics.size(); k++) {
                    if (expectedPics[k] == String(name)) {
                        keep = true;
                        break;
                    }
                }

                if (!keep) {
                    // orphan -> delete
                    String fullPath = "/qspi/";
                    fullPath += name;

                    Serial.print("[PICS] Deleting orphan: ");
                    Serial.println(fullPath);

                    deleteFileFromQSPI( fullPath );
                    picsDeleted++;
                }
            }

            closedir(dir);
        }

        // 4) Download any expected pics that do not exist yet
        for (size_t i = 0; i < expectedPics.size(); i++) {

            String fname = expectedPics[i];
            String path  = "/qspi/";
            path += fname;

            // check existence
            FILE* f = openFileFromQSPI(path);
            if (f != NULL) {
                // already cached
                closeFileFromQSPI(f);
                Serial.print("[PICS] Already present: ");
                Serial.println(path);
                continue;
            }

            // we need to download; extract uuid from "brickimg_<uuid>.jpg"
            String uuid = fname;
            // remove "brickimg_" prefix
            uuid.remove(0, 9);
            // remove ".jpg" suffix if present
            int dotPos = uuid.lastIndexOf(".jpg");
            if (dotPos >= 0) {
                uuid = uuid.substring(0, dotPos);
            }

            Serial.print("[PICS] Missing, downloading uuid: ");
            Serial.println(uuid);

            size_t imgLen = 0;

            
            uint8_t* img; 
            WiFiSSLClient serverConnection = domainManagerClass::getInstance()->comms->connectToServer(domainManagerClass::getInstance()->serverURL);                                                                
            img = downloadImageToSDRAM(serverConnection, uuid, imgLen);
            if (img == NULL || imgLen == 0) {
                Serial.println("[PICS] downloadImageToSDRAM returned null/0");
                throw std::runtime_error("syncPics: downloadImageToSDRAM failed");
            }

            // Save to QSPI, always free img
            Serial.println("Save to disk!!!");
            try {
                saveBinaryFileFromBuffer(path, img, imgLen);
            } catch (...) {
                SDRAM.free(img);
                throw;
            }

            SDRAM.free(img);

            Serial.print("[PICS] Cached ");
            Serial.print(imgLen);
            Serial.print(" bytes to ");
            Serial.println(path);

            picsLoaded++;
        }

        Serial.print("SYNC PICS DONE, loaded: ");
        Serial.print(picsLoaded);
        Serial.print(", deleted orphans: ");
        Serial.println(picsDeleted);

    
        return picsLoaded;
    }
    //----------------

    void zapPics() {

        Serial.println("ZAP PICS (DIR SCAN) =========================================");

        // Ensure QSPI filesystem is accessible.
        openQSPI();
        DIR* dir = openDirFromQSPI();

        struct dirent* de;
        int removedCount = 0;

        while ((de = readdir(dir)) != NULL) {
            const char* name = de->d_name;

            // Ignore "." and ".." and directories
            if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'))) {
                continue;
            }
            if (de->d_type == DT_DIR) {
                continue;
            }

            // Only delete our own files: brickimg_*.*.
            const char* prefix = "brickimg_";
            int prefixLen = 9; // strlen("brickimg_");
            bool isBrickImg = true;
            for (int i = 0; i < prefixLen; i++) {
                if (name[i] != prefix[i]) {
                    isBrickImg = false;
                    break;
                }
            }
            if (!isBrickImg) {
                continue;
            }

            String fullPath = "/qspi/";
            fullPath += name;

            Serial.print("[ZAP PICS] Removing: ");
            Serial.println(fullPath);

            int rc = remove(fullPath.c_str());
            if (rc == 0) {
                Serial.println("[ZAP PICS]   OK (deleted)");
                removedCount++;
            } else {
                Serial.println("[ZAP PICS]   ERROR (could not delete)");
            }
        }

        
        closeDirFromQSPI( dir );

        Serial.print("ZAP PICS DONE, total removed: ");
        Serial.println(removedCount);
        Serial.println("=============================================================");
    }

    //==========================================================================================================================================
    //==========================================================================================================================================
    //==========================================================================================================================================    

};


