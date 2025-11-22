/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/

void navigateTo(int screenId);
extern String BEARER_TOKEN;

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
    std::vector<  std::vector<String>  > components;    
    layoutZoneClass(String nameParam, String tagParam):name(nameParam),tag(tagParam){}
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
        const String& zoneName,
        const String& componentName,
        const String& defectType,
        int severity,
        const String& notes,
        const String& time)
    : asset(assetParam),
      zoneName(zoneName),
      componentName(componentName),
      defectType(defectType),
      severity(severity),
      notes(notes),
      time(time)
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

    String submitTime;
    String startTime;
    String offset;
    String dst;

    String driver_username;    
    String driver_name;

    bool submitted =  false;
    String serverReply = "";

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

        submitTime = "";
        startTime = "";
        offset = "";
        dst = "";

        driver_username = "";        
        driver_name = "";

        submitted = false;
        serverReply = "";
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

        result += "Submit time: ";
        result += submitTime;
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

        result += "\nServer reply:\n\n";
        result += serverReply + "\n\n";

        return result;
    }   
    
    //====================================
    //============
    //====================================

    String toEDI() const {

        String result = "BRICKINSPECTION*1\n";

        // display header 
        result += 
            "DISPLAYHEADER*" 
            + String(rtc->now().unixtime()) +
            + "*" + id.substring(id.length() - 5) + "(" + BEARER_TOKEN.substring(BEARER_TOKEN.length() - 5) + ")"
            + "*" + submitTime 
            + "*" + driver_name 
            + "*" + assets[0].buttonName
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
        result += submitTime;
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
                result += inspectionFormFieldValues[rowIndex];
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
                result += "*" + defect.notes;
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
                result += "*" + defect.notes;
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
    std::vector<userClass> users;    
    std::vector<assetClass> assets;
    std::vector<layoutClass> layouts;
    std::vector<inspectionTypeClass> inspectionTypes;    


//----------------------------

public:

    // Const reference getters
    const std::vector<userClass>* getUsers() const { return &users; }
    const std::vector<assetClass>* getAssets() const { return &assets; }
    const std::vector<layoutClass>* getLayouts() const { return &layouts; }
    const std::vector<inspectionTypeClass>* getInspectionTypes() const { return &inspectionTypes; }

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

                comms->connectToWifi();
                std::vector<String> config = comms->GET( serverURL , getConfigPath + "?company=" + domainManagerClass::getInstance()->company );

                parse( &config );
                saveToKVStore( "/kv/config", &config );                

                    String syncMessage = "Sync successful. \n";

                    syncMessage += domainManagerClass::getInstance()->assets.size();
                    syncMessage += " assets, ";

                    syncMessage += domainManagerClass::getInstance()->layouts.size();
                    syncMessage += " layouts, ";

                    syncMessage += domainManagerClass::getInstance()->inspectionTypes.size();
                    syncMessage += " types, ";

                    syncMessage += domainManagerClass::getInstance()->users.size();
                    syncMessage += " users. ";       

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

        emptyAll();

        std::vector<String>::iterator iterator = config->begin();    
        while ( iterator != config->end() ) {   

            // HEADER ----->
            if (++iterator == config->end()) throw std::runtime_error("Unexpected end ");            
            std::vector<String> tokens = tokenize( *iterator , '*' );     
            if( tokens[ 0 ] == "BRICKCONFIG" ){ 
                Serial.println( "[BRICKCONFIG] found..." );

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
                            if( tokens.size() == 3 ){
                                String zoneTag = tokens[ 1 ];
                                String zoneName = tokens[ 2 ];

                                // make layout step 2
                                layoutZoneClass zone(zoneName,zoneTag);

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

                            }else throw std::runtime_error( "Parse error token LAYZONE expecting 3 tokens " );
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


};


