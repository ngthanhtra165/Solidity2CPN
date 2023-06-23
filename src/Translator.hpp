#ifndef SOL2CPN_TRANSLATOR_H_
#define SOL2CPN_TRANSLATOR_H_

#include "../include/Helena.hpp"
#include "../include/nlohmann/json.hpp"
#include "ASTNodes.hpp"
#include <set>

using namespace HELENA;
namespace SOL2CPN {

const string PlusSign = "+";
const string MinusSign = "-";
const string DividerSign = "/";
const string MultiplySign = "*";
const string EqualSign = "=";
const string DoublePlussSign = "++";
const string DoubleMinusSign = "--";
const string DividerAndEqualSign = "/=";
const string PlusAndEqualSign = "+=";
const string MinusAndEqualSign = "-=";
const string MultiplyAndEqualSign = "*=";
const string GreaterSign = ">";
const string LessSign = "<";
const string GreaterOrEqualSign = ">=";
const string LessOrEqualSign = "<=";
const string NotSign = "!";
const string AndSign = "&&";
const string OrSign = "||";
const string NotEqualSign = "!=";
const string DoubleEqualSign = "==";
const string AssignSign = "::";

class FunctionNet{
public:
    FunctionNet(std::string _name, std::string _scname): name(_name), smartcontract_name(_scname) {}
    std::string get_name();

	void set_name(const std::string& _name);

    void add_color(const ColorNodePtr& _color);
    ColorNodePtr get_color(const unsigned int& x);
	ColorNodePtr get_color_by_name(const std::string& _name);
    size_t num_colors();

    void add_local_variable(const std::string& _name, const PlaceNodePtr& _place);
    bool is_local_variable(const std::string& _name);
    PlaceNodePtr get_local_variable_by_name(const std::string& _name);

    void set_param_place(const PlaceNodePtr& _place);
	PlaceNodePtr get_param_place();

	void set_input_place(const PlaceNodePtr& _place);
	PlaceNodePtr get_input_place();

    void add_place(const PlaceNodePtr& _place);
    PlaceNodePtr get_place(const unsigned int& x);
    size_t num_places();

    void add_transition(const TransitionNodePtr& _trans);
    TransitionNodePtr get_transition(const unsigned int& x);
    size_t num_transitions();

    void add_parameter(const std::string& name, const std::string& type);
    bool is_parameter(const std::string& name);
    std::string get_parameter_type(const std::string& name);

    PlaceNodePtr generateControlFlowPlace(const std::string& _str);
    std::string generateTransitionName(const std::string& _str);
    std::string generatePlaceName(const std::string& _str);
private:
    std::vector<ColorNodePtr> color_nodes;

    std::vector<PlaceNodePtr> place_nodes;
    std::map<std::string, int> place_node_names;

    std::map<std::string, PlaceNodePtr> local_variables;

    PlaceNodePtr param_place;
	PlaceNodePtr input_place;

    std::map<std::string, std::string> param_variables;

    std::vector<TransitionNodePtr> transition_nodes;
    std::map<std::string, int> transition_node_names;

    std::string name;

	std::string smartcontract_name;
};
typedef std::shared_ptr<FunctionNet> FunctionNetPtr;


struct UserDefineVariable {
    ColorNodePtr color;
    NodeType type;
};

struct SmartContractObject{
	PlaceNodePtr place;
	std::string type;
};

class SmartContractNet{
public:    
    SmartContractNet(std::string _name);

    std::string get_name();
	StructColorNodePtr get_state_color();

    void add_color(const ColorNodePtr& _color);
    ColorNodePtr get_color(const unsigned int& x);
    ColorNodePtr get_color_by_name(const std::string& _name);
    size_t num_colors();

    void add_state_color(const ColorNodePtr& _color,const std::string& _var_name);
    bool is_state_color(const std::string& _name);
    ColorNodePtr get_state_color_by_name(const string& _name);

    void add_function(const FunctionNetPtr& _func);
    FunctionNetPtr get_function_by_name(const std::string& _func_name);

    void add_function_node(const FunctionNodePtr& _func);
    FunctionNodePtr get_function_node(const unsigned int& x);
    size_t num_function_nodes();

    void add_defined_color(const std::string name, const ColorNodePtr& color, const NodeType& type);
    bool is_defined_color(const std::string name);
    UserDefineVariable* get_defined_color_by_name(const std::string name);

	void add_smartcontract_obj(const std::string name, const PlaceNodePtr& place, const std::string type);
	bool is_smartcontract_obj(const std::string name);
	SmartContractObject* get_smartcontract_obj_by_name(const std::string name);

	PlaceNodePtr get_func_key();

	PlaceNodePtr get_state_place();

	void set_state_placeM(const PlaceNodePtr& _place);
	PlaceNodePtr get_state_placeM();

	void set_has_constructor();
	bool is_has_constructor();

	void set_constructor(const FunctionNetPtr& _func);
	FunctionNetPtr get_constructor();

    std::string convertColorInit(const ColorNodePtr& _color);
    std::string source_code();

private:
    std::string name;
	
	bool has_constructor;
	PlaceNodePtr func_key;

    std::vector<ColorNodePtr> default_color_nodes;
    std::vector<ColorNodePtr> color_nodes;
    std::map<std::string, UserDefineVariable*> defined_color_nodes;
    std::map<std::string, ColorNodePtr> stateColor;
	std::map<std::string, SmartContractObject*> smartcontract_obj;

	StructColorNodePtr stateColor_s;
	PlaceNodePtr statePlace;
	PlaceNodePtr statePlaceM;
    std::vector<FunctionNodePtr> function_nodes;
    std::vector<FunctionNetPtr> functionNets;
	FunctionNetPtr constructor;
};
typedef std::shared_ptr<SmartContractNet> SmartContractNetPtr;

const std::string PlaceParType = "place_par";
const std::string PlaceStateType = "place_state";
const std::string PlaceStateParType = "place_state_par";
const std::string PlaceVarialeLocalType = "place_local_var";
const std::string PlaceNoneType = "";

const std::string VariableGlobalType = "global";
const std::string VariableLocalType = "local";
const std::string VariableFunctionParmType = "func_par";
const std::string VariableNonType = "";


struct temp_variable {
    int block_num;
    PlaceNodePtr place;
    std::string name;
};


class Translator {
public:
    Translator(RootNodePtr _rootNode) : rootNode(_rootNode) {}
    std::string translate();
    

private:
    RootNodePtr rootNode;
    NetNodePtr net;
    SmartContractNetPtr currentCNet;
    FunctionNetPtr currentFNet;
    std::vector<PlaceNodePtr> in_places;
    std::vector<temp_variable*> tv_;
	std::vector<SmartContractNetPtr> list_smartcontract;

	SmartContractNetPtr get_smartcontractnet_by_name(const std::string& _name);

	std::string getInStateType();
    std::string getStateInArc();
    std::string getParInArc();
    ArcNodePtr generateStateOutArc();
    ArcNodePtr generateStateParOutArc(const PlaceNodePtr& _place);
    std::string getPlaceTypeInFlow(const PlaceNodePtr _place);
    std::string generateArcInLabel(const PlaceNodePtr& _place,const std::string& _place_type);
    ArcNodePtr generateArcInByPlace(const PlaceNodePtr& _place,const std::string& _place_type);

    StructColorNodePtr translateStruct(const StructDefinitionNodePtr& struct_node);
    StructColorNodePtr translateEnum(const EnumDefinitionNodePtr& enum_node);
    ListColorNodePtr translateMapping(const ASTNodePtr& _node);
    ListColorNodePtr translateArray(const ArrayTypeNameNodePtr& array_node);
    PlaceNodePtr translateVariableDeclaration(const ASTNodePtr& _var,const std::string& _type, const int block_num);
    FunctionNodePtr createGetMappingIndexFunction(const std::string& name, const ParamNodePtr& param1, const ParamNodePtr& param2);
    
	void createFunctionInputTransition(const FunctionNetPtr& functionNet);
    void createFunctionColorPlace(const FunctionDefinitionNodePtr& _func);
    void translateBlock(const BlockNodePtr& block, const int block_num);
    void translateExpressionStatement(const ExpressionStatementNodePtr& statement, const int block_num);
    void translate_equivalent_node(const ASTNodePtr& _node, const int block_num);
    void translateFunctionCall(const FunctionCallNodePtr& func_call, const int block_num);
	void translateObjFunctionCall(const std::string& obj_name, const std::string& func_name, const FunctionCallNodePtr& func_call, const int block_num);
    void translateRequireFunctionCall(const ASTNodePtr& _arg, const int block_num);
	void translateRevertFunctionCall(const ASTNodePtr& _arg, const int block_num);
    void translateVariableDeclarationStatement(const VariableDeclarationStatementNodePtr& statement, const int block_num);
    void translateIfStatement(const IfStatementNodePtr& statement, const int block_num);
    void translateIfcondition(const IfStatementNodePtr& statement, const PlaceNodePtr& end_if, const int block_num);
    void translateAssignment(const AssignmentNodePtr& assignment, const int block_num);
    /* void translateTransferFunctionCall(const ASTNodePtr& identifier, const ASTNodePtr& argument);
    void translatePushFunctionCall(const ASTNodePtr& identifier, const ASTNodePtr& argument); */
    void translateForStatement(const ForStatementNodePtr& forStatement, const int block_num);

	std::string translateAddressTypeValue(const std::string& _value);
    std::string translateRequireExpressionToString(const ASTNodePtr& _arg, const int block_num);
    std::string translateOperator(const std::string& _operator, const std::string& member);
    std::string translateMemberToString(const ASTNodePtr& _arg);
    std::string translateMemberAcess(const std::string& _member);
    std::vector<std::string> createChangeValueLet(const std::string& name, const std::string& left_member, const std::string& right_member, const std::string& op);

    static temp_variable* generateTempVariableStruct(const std::string& name, const int& block_num, const PlaceNodePtr& place);
    temp_variable* get_temp_variable(const std::string& name, const int block_num);
    bool is_temp_variable(const std::string& name, const int block_num);
    bool is_temp_variable_place(const PlaceNodePtr& place, const int block_num);

    void add_in_places(const PlaceNodePtr& place);

	void postProcessingSmartContractNet(const SmartContractNetPtr& smartcontractNet);
	std::string createInitColor(const ColorNodePtr& _color, const SmartContractNetPtr& smartcontractNet, NodeType type);

    std::map<std::string,std::string> DataTypeRange{
        {"UINT","range 0 .. (int'last)"},
		{"UINT256","range 0 .. (int'last)"},
        {"UINT8","range 0 .. 255"},
		{"INT","range (int'first) .. (int'last)"},
        {"BYTES32","range 0 .. (int'last)"},
        {"ADDRESS","range 0 .. 1000"},
        {"BOOL","enum(true,false)"},
    };

    std::map<std::string, std::string> NormalSign = {
        {PlusSign,"+"},
        {MinusSign,"-"},
        {DividerSign,"/"},
        {MultiplySign,"*"},
        {EqualSign,":="},
        {DoubleEqualSign,"="},
        {GreaterSign,">"},
        {LessSign,"<"},
        {GreaterOrEqualSign,">="},
        {LessOrEqualSign,"<="},
        {NotSign,"not"},
        {AndSign,"and"},
        {OrSign,"or"},
        {NotEqualSign,"!="}
    };

    std::vector<std::string> SpecialSign = {
        DoublePlussSign,
        DoubleMinusSign,
        DividerAndEqualSign,
        PlusAndEqualSign,
        MinusAndEqualSign,
        MultiplyAndEqualSign
    };

    bool isNormalSign(std::string sign);
    bool isSpecialSign(std::string sign);

};

}

#endif //SOL2CPN_TRANSLATOR_H_