/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/


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

    defectClass(
        assetClass assetParam,
        const String& zoneName,
        const String& componentName,
        const String& defectType,
        int severity,
        const String& notes)
    : asset(assetParam),
      zoneName(zoneName),
      componentName(componentName),
      defectType(defectType),
      severity(severity),
      notes(notes)
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
    inspectionTypeClass* type = NULL;
    std::vector<assetClass> assets;    
    std::vector<defectClass> defects;
    std::vector<  String  > inspectionFormFieldValues; 
    String submitTime;

    String driver_username;    
    String driver_name;

    inspectionClass(){}

    void clear() {
        type = NULL;
        assets.clear();
        defects.clear();
        inspectionFormFieldValues.clear();
        submitTime = "";
        driver_username = "";        
        driver_name = "";
    }

    String toString() const {
        String result = "INSPECTION\n";

        // --- Inspection Type ---
        if (type != NULL) {
            result += "Type: ";
            result += type->name;
            result += "\n";

            result += "Driver: ";
            result += driver_username;
            result += " ";       
            result += driver_name;
            result += "\n";            

            result += "Layouts:\n";
            for (const auto& layout : type->layouts) {
                result += " - ";
                result += layout;
                result += "\n";
            }

            result += "Form Fields:\n";
            size_t rowIndex = 0;
            for (const auto& row : type->formFields) {
                result += "  Row ";
                result += String(rowIndex);
                result += ": ";
                for (size_t i = 0; i < row.size(); ++i) {
                    result += row[i];
                    if (i < row.size() - 1) {
                        result += ", ";
                    }
                }
                // Add the value if it exists
                if (rowIndex < inspectionFormFieldValues.size()) {
                    result += " => ";
                    result += inspectionFormFieldValues[rowIndex];
                } else {
                    result += " => <unset>";
                }
                result += "\n";
                ++rowIndex;
            }

        } else {
            result += "Type: NULL\n";
        }

        result += "Submit time: ";
        result += submitTime;
        result += "\n";        

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
        result += "Defects (sev == 0):\n";
        for (const auto& defect : defects) {
            if (defect.severity == 0) {
                result += " - Asset ID: ";
                result += defect.asset.ID;
                result += ", Zone: " + defect.zoneName;
                result += ", Component: " + defect.componentName;
                result += ", Type: " + defect.defectType;
                result += ", Severity: " + String(defect.severity);
                result += ", Notes: " + defect.notes;
                result += "\n";
            }
        }

        // --- Defects: severity > 0 after ---
        result += "Defects (sev > 0):\n";
        for (const auto& defect : defects) {
            if (defect.severity > 0) {
                result += " - Asset ID: ";                
                result += defect.asset.ID;
                result += ", Zone: " + defect.zoneName;
                result += ", Component: " + defect.componentName;
                result += ", Type: " + defect.defectType;
                result += ", Severity: " + String(defect.severity);
                result += ", Notes: " + defect.notes;
                result += "\n";
            }
        }

        return result;
    }        
};

//-------------------------------------------------

// END          D O M A I N 

//-------------------------------------------------




