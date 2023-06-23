#include "Translator.hpp"
#include "../include/Utils.hpp"

namespace SOL2CPN {
    
bool Translator::isNormalSign(std::string sign){
    if(NormalSign.find(sign) != NormalSign.end()){
        return true;
    }
    return false;
}

bool Translator::isSpecialSign(std::string sign){
    if (std::find(SpecialSign.begin(), SpecialSign.end(), sign) != SpecialSign.end())
    {
        return true;
    }
    return false;
}

/** Function Net **/

/** get the net's name
 */
std::string FunctionNet::get_name(){
    return name;
}
/** set the net's name
 */
void FunctionNet::set_name(const std::string& _name) {
	name = _name;
}
/** Add color to the net
 */
void FunctionNet::add_color(const ColorNodePtr& _color) {
    color_nodes.push_back(_color);
}
/** Get a color of the net
 */
ColorNodePtr FunctionNet::get_color(const unsigned int& x) {
    return color_nodes[x];
}
/** Get a color of the net by name
 */
ColorNodePtr FunctionNet::get_color_by_name(const std::string& _name) {
	for (auto it = color_nodes.begin(); it != color_nodes.end(); it++) {
		if ((*it)->get_name() == _name) {
			return (*it);
		}
	}
	return nullptr;
}

/** Get the number of color in net
 */
size_t FunctionNet::num_colors() {
    return color_nodes.size();
}

void FunctionNet::add_local_variable(const std::string& _name, const PlaceNodePtr& _place){
    if(!is_local_variable(_name)){
        local_variables[_name] = _place;
    }
}

bool FunctionNet::is_local_variable(const std::string& _name){
    if(local_variables.find(_name) == local_variables.end()){
        return false;
    }
    return true;
}

PlaceNodePtr FunctionNet::get_local_variable_by_name(const std::string& _name){
    if(is_local_variable(_name)){
        return local_variables[_name];
    }
    return nullptr;
}

void FunctionNet::set_param_place(const PlaceNodePtr& _place){
    param_place = _place;
}

PlaceNodePtr FunctionNet::get_param_place() {
	return param_place;
}

void FunctionNet::set_input_place(const PlaceNodePtr& _place) {
	input_place = _place;
}

PlaceNodePtr FunctionNet::get_input_place() {
	return input_place;
}

void FunctionNet::add_place(const PlaceNodePtr& _place){
    std::string place_name = _place->get_name();
    int count = place_name.back() - '0';
    place_name.pop_back();

    place_node_names[place_name] = count;
    place_nodes.push_back(_place);
}

PlaceNodePtr FunctionNet::get_place(const unsigned int& x){
    return place_nodes[x];
}

size_t FunctionNet::num_places(){
    return place_nodes.size();
}

PlaceNodePtr FunctionNet::generateControlFlowPlace(const std::string& _str){
    std::string place_name = generatePlaceName(_str);

    PlaceNodePtr cflow = std::make_shared<PlaceNode>();
    cflow->set_name(place_name);
    cflow->set_domain(to_upper_copy(name)+"_STATE");

    return cflow;
}

void FunctionNet::add_transition(const TransitionNodePtr& _trans){
    std::string trans_name = _trans->get_name();
    int count = trans_name.back() - '0';
    trans_name.pop_back();

    transition_node_names[trans_name] = count;
    transition_nodes.push_back(_trans);
}

TransitionNodePtr FunctionNet::get_transition(const unsigned int& x){
    return transition_nodes[x];
}

size_t FunctionNet::num_transitions(){
    return transition_nodes.size();
}

void FunctionNet::add_parameter(const std::string& name, const std::string& type){
    param_variables[name] = type;
}

bool FunctionNet::is_parameter(const std::string& name){
    if(param_variables.find(name) != param_variables.end()){
        return true;
    }
    return false;
}

std::string FunctionNet::get_parameter_type(const std::string& name){
    if(is_parameter(name)){
        return param_variables[name];
    }
    return "";
}


std::string FunctionNet::generatePlaceName(const std::string& _str){
    std::string new_name = smartcontract_name + to_upper_copy(name)+_str;
    int current_count;
    if(place_node_names.find(new_name) != place_node_names.end()){
        current_count = place_node_names[new_name];
    }else{
        current_count = 0;
    }
    
    return new_name + std::to_string(current_count+1);
}

std::string FunctionNet::generateTransitionName(const std::string& _str){
    std::string new_name = smartcontract_name + to_upper_copy(name)+_str;
    int current_count;
    if(transition_node_names.find(new_name) != transition_node_names.end()){
        current_count = transition_node_names[new_name];
    }else{
        current_count = 0;
    }
    
    return new_name + std::to_string(current_count+1);
}

/** SmartContract Net **/
SmartContractNet::SmartContractNet(std::string _name){
    name = _name;
	stateColor_s = std::make_shared<StructColorNode>();
	stateColor_s->set_name(to_upper_copy(name) + "_STATE");

    ColorNodePtr address = std::make_shared<ColorNode>();
    address->set_name("ADDRESS");
    address->set_typeDef("range 0 .. 1000");
    
    ColorNodePtr cuint = std::make_shared<ColorNode>();
    cuint->set_name("UINT");
    cuint->set_typeDef("range 0 .. (int'last)"); 

    StructColorNodePtr user = std::make_shared<StructColorNode>();
    user->set_name("USER");

    ComponentNodePtr acomponent = std::make_shared<ComponentNode>();
    acomponent->set_name("adr");
    acomponent->set_type("ADDRESS");
    user->add_component(acomponent);

    ComponentNodePtr bcomponent = std::make_shared<ComponentNode>();
    bcomponent->set_name("balance");
    bcomponent->set_type("UINT");
    user->add_component(bcomponent);

    default_color_nodes.push_back(address);
    default_color_nodes.push_back(cuint);
    default_color_nodes.push_back(user);

	add_color(address);
	add_color(cuint);
	add_color(user);


	func_key = std::make_shared<PlaceNode>();
	func_key->set_name(to_upper_copy(name) + "_FUNCKEY");
	func_key->set_domain("epsilon");
	func_key->set_init("epsilon");

	statePlace = std::make_shared<PlaceNode>();
	statePlace->set_name(to_upper_copy(name) + "_S");
	statePlace->set_domain(stateColor_s->get_name());

	statePlaceM = std::make_shared<PlaceNode>();
	statePlaceM->set_name(to_upper_copy(name) + "_SM");
	statePlaceM->set_domain(stateColor_s->get_name());
}
/** get the net's name
 */
std::string SmartContractNet::get_name(){
    return name;
}
/** get the net's stateColor_s
 */
StructColorNodePtr SmartContractNet::get_state_color() {
	return stateColor_s;
}
/** Add color to the net
 */
void SmartContractNet::add_color(const ColorNodePtr& _color) {
    color_nodes.push_back(_color);
}
/** Get a color of the net
 */
ColorNodePtr SmartContractNet::get_color(const unsigned int& x) {
    return color_nodes[x];
}
/** Get a color by name
 */
ColorNodePtr SmartContractNet::get_color_by_name(const std::string& _name){
    for(auto it = color_nodes.begin(); it != color_nodes.end(); it++){
        if((*it)->get_name() == _name){
            return (*it);
        }
    }
    return nullptr;
}
/** Get the number of color in net
 */
size_t SmartContractNet::num_colors() {
    return color_nodes.size();
}

void SmartContractNet::add_function_node(const FunctionNodePtr& _func){
    function_nodes.push_back(_func);
}

FunctionNodePtr SmartContractNet::get_function_node(const unsigned int& x){
    return function_nodes[x];
}

size_t SmartContractNet::num_function_nodes(){
    return function_nodes.size();
}

void SmartContractNet::add_defined_color(const std::string name, const ColorNodePtr& color, const NodeType& type){
    UserDefineVariable* new_color = new UserDefineVariable();
    new_color->color = color;
    new_color->type = type;
    defined_color_nodes[name] = new_color;
}

bool SmartContractNet::is_defined_color(const std::string name){
    if(defined_color_nodes.find(name) != defined_color_nodes.end()){
        return true;
    }
    return false;
}


UserDefineVariable* SmartContractNet::get_defined_color_by_name(const std::string name){
    if(is_defined_color(name)){
        return defined_color_nodes[name];
    }
    return NULL;
}

void SmartContractNet::add_smartcontract_obj(const std::string name, const PlaceNodePtr& place, const std::string type) {
	SmartContractObject* new_obj = new SmartContractObject();
	new_obj->place = place;
	new_obj->type = type;
	smartcontract_obj[name] = new_obj;
}

bool SmartContractNet::is_smartcontract_obj(const std::string name) {
	if (smartcontract_obj.find(name) != smartcontract_obj.end()) {
		return true;
	}
	return false;
}


SmartContractObject* SmartContractNet::get_smartcontract_obj_by_name(const std::string name) {
	if (is_smartcontract_obj(name)) {
		return smartcontract_obj[name];
	}
	return NULL;
}


PlaceNodePtr SmartContractNet::get_func_key() {
	return func_key;
}

PlaceNodePtr SmartContractNet::get_state_place() {
	return statePlace;
}

void SmartContractNet::set_state_placeM(const PlaceNodePtr& _place) {
	statePlaceM = _place;
}

PlaceNodePtr SmartContractNet::get_state_placeM() {
	return statePlaceM;
}

void SmartContractNet::set_has_constructor() {
	has_constructor = true;
}

bool SmartContractNet::is_has_constructor() {
	return has_constructor;
}

void SmartContractNet::set_constructor(const FunctionNetPtr& _func) {
	constructor = _func;
}

FunctionNetPtr SmartContractNet::get_constructor() {
	return constructor;
}

void SmartContractNet::add_state_color(const ColorNodePtr& _color,const std::string& _var_name){
    if(!is_state_color(_var_name))
    {
        stateColor[_var_name] = _color;

		ComponentNodePtr com = std::make_shared<ComponentNode>();
		com->set_type(_color->get_name());
		com->set_name(_var_name);

		stateColor_s->add_component(com);
    }
}

bool SmartContractNet::is_state_color(const std::string& _name){
    if(stateColor.find(_name) == stateColor.end()){ 
        return false;
    }
    return true;
}

ColorNodePtr SmartContractNet::get_state_color_by_name(const string& _name){
    if(is_state_color(_name)){
        return stateColor[_name];
    }
    return nullptr;
}

void SmartContractNet::add_function(const FunctionNetPtr& _func){
    functionNets.push_back(_func);
}

FunctionNetPtr SmartContractNet::get_function_by_name(const std::string& _func_name){
    for(auto it = functionNets.begin(); it != functionNets.end(); it++){
        if((*it)->get_name() == _func_name){
            return *it;
        }
    }
    return nullptr;
}

std::string SmartContractNet::convertColorInit(const ColorNodePtr& _color){
    if(_color != nullptr && _color->get_node_type() == LnaNodeTypeColor){
        return _color->get_init_value();
    }else if(_color != nullptr && _color->get_node_type() == LnaNodeTypeStruct_Color){
        return "";
    }else if(_color != nullptr && _color->get_node_type() == LnaNodeTypeListColor){
        return "";
    }
    return "";
}


std::string SmartContractNet::source_code(){
    StructuredNetNodePtr net = std::make_shared<StructuredNetNode>(name);
    std::map<std::string, bool> colors;

    net->add_color(std::make_shared<CommentNode>("\n// DEFAULT COLORS DEFINITIONS\n"));
    for(auto it = default_color_nodes.begin(); it != default_color_nodes.end(); it++){
        if(colors.find((*it)->get_name()) == colors.end()){
            colors[(*it)->get_name()] = true;
            net->add_color(*it);
        }
    }

    net->add_color(std::make_shared<CommentNode>("\n// SMARTCONTRACT COLORS DEFINITIONS\n"));
    for(auto it = color_nodes.begin(); it != color_nodes.end(); it++){
        if(colors.find((*it)->get_name()) == colors.end()){
            colors[(*it)->get_name()] = true;
            net->add_color(*it);
        }
    }

    net->add_color(std::static_pointer_cast<ColorNode>(stateColor_s));


    for(auto it = function_nodes.begin(); it != function_nodes.end(); it++){
        net->add_function(*it);
    }

	net->add_place(std::make_shared<CommentNode>("\n/*\n * function key\n */\n"));
	net->add_place(func_key);

	PlaceNodePtr block_place = std::make_shared<PlaceNode>();
	block_place->set_name("BLOCK_PLACE");
	block_place->set_domain("epsilon");
	net->add_place(std::make_shared<CommentNode>("\n/*\n * block place\n */\n"));
	net->add_place(block_place);

    net->add_place(std::make_shared<CommentNode>("\n/*\n * Function: state\n */\n"));
    net->add_place(statePlace);

	if (is_has_constructor()) {
		net->add_place(statePlaceM);
	}

	ArcNodePtr block_arc = std::make_shared<ArcNode>();
	block_arc->set_placeName(block_place->get_name());
	block_arc->set_label("epsilon");


    for(auto it = functionNets.begin(); it != functionNets.end(); it++){
        for(size_t i = 0; i < (*it)->num_colors(); i++){
            ColorNodePtr color = (*it)->get_color(i);
            net->add_color(color);
        }
        net->add_place(std::make_shared<CommentNode>("\n/*\n * Function: " + (*it)->get_name() + "\n */\n"));
        for(size_t i = 0; i < (*it)->num_places(); i++){
            PlaceNodePtr place = (*it)->get_place(i);
            net->add_place(place);
        }
        net->add_transition(std::make_shared<CommentNode>("\n/*\n * Function: " + (*it)->get_name() + "\n */\n"));
        for(size_t i = 0; i < (*it)->num_transitions(); i++){
            TransitionNodePtr trans = (*it)->get_transition(i);

			if (trans->num_inArc() == 0) {
				TransitionNodePtr empty_trans = std::make_shared<TransitionNode>();
				empty_trans->set_name(trans->get_name());
				empty_trans->add_inArc(block_arc);
				net->add_transition(empty_trans);
			}
			else {
				net->add_transition(trans);
			}
        }
    }
    return net->source_code();
}

SmartContractNetPtr Translator::get_smartcontractnet_by_name(const std::string& _name) {
	for (auto it = list_smartcontract.begin(); it != list_smartcontract.end(); it++) {
		if ((*it)->get_name() == _name) {
			return *it;
		}
	}
	return nullptr;
}

/** Start translate
 */
std::string Translator::translate() {
	net = std::make_shared<NetNode>();

	for (int i = 0; i < rootNode->num_fields(); i++) {
		ASTNodePtr field = rootNode->get_field(i);
		if (field != nullptr && field->get_node_type() == NodeTypeContractDefinition) {
			//create the net
			auto contractNode = std::static_pointer_cast<ContractDefinitionNode>(field);
			SmartContractNetPtr contractNet = std::make_shared<SmartContractNet>(contractNode->get_name());
			list_smartcontract.push_back(contractNet);
		}
	}

	for (int i = 0; i < rootNode->num_fields(); i++) {
		ASTNodePtr field = rootNode->get_field(i);
		if (field != nullptr && field->get_node_type() == NodeTypeContractDefinition) {
			//create the net
			auto contractNode = std::static_pointer_cast<ContractDefinitionNode>(field);

			if (contractNode->get_contract_kind() == SmartContractKindContract) {
				currentCNet = get_smartcontractnet_by_name(contractNode->get_name());;

				for (int j = 0; j < contractNode->num_members(); j++) {
					auto member = contractNode->get_member(j);

					if (member != nullptr) {
						if (member->get_node_type() == NodeTypeStructDefinition) {
							auto structNode = std::static_pointer_cast<StructDefinitionNode>(member);
							StructColorNodePtr struct_type = translateStruct(structNode);
							currentCNet->add_color(std::static_pointer_cast<ColorNode>(struct_type));
							currentCNet->add_defined_color(structNode->get_name(), struct_type, NodeTypeStructDefinition);
						}
						else if (member->get_node_type() == NodeTypeEnumDefinition) {
							auto enumNode = std::static_pointer_cast<EnumDefinitionNode>(member);
							StructColorNodePtr enum_type = translateEnum(enumNode);
							currentCNet->add_color(enum_type);

							currentCNet->add_state_color(enum_type, enumNode->get_name());
							currentCNet->add_defined_color(enumNode->get_name(), enum_type, NodeTypeEnumDefinition);
						}
						else if (member->get_node_type() == NodeTypeVariableDeclaration) {
							translateVariableDeclaration(member, VariableGlobalType, 0);
						}
						else if (member->get_node_type() == NodeTypeFunctionDefinition) {
							FunctionDefinitionNodePtr func = std::static_pointer_cast<FunctionDefinitionNode>(member);
							FunctionNetPtr functionNet = std::make_shared<FunctionNet>(func->get_name(), std::to_string(i));

							if (func->function_is_constructor()) {
								func->set_name("constructor");
								functionNet->set_name("constructor");
								currentCNet->set_has_constructor();
								currentCNet->set_constructor(functionNet);
							}

							currentFNet = functionNet;

							createFunctionColorPlace(func);

							if (!func->function_is_constructor()) {
								createFunctionInputTransition(functionNet);
							}
							currentCNet->add_function(currentFNet);
							in_places.clear();
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < rootNode->num_fields(); i++) {
		ASTNodePtr field = rootNode->get_field(i);
		if (field != nullptr && field->get_node_type() == NodeTypeContractDefinition) {
			//create the net
			auto contractNode = std::static_pointer_cast<ContractDefinitionNode>(rootNode->get_field(i));

			if (contractNode->get_contract_kind() == SmartContractKindContract) {
				currentCNet = get_smartcontractnet_by_name(contractNode->get_name());

				for (int j = 0; j < contractNode->num_members(); j++) {
					auto member = contractNode->get_member(j);

					if (member != nullptr && member->get_node_type() == NodeTypeFunctionDefinition) {

						FunctionDefinitionNodePtr func = std::static_pointer_cast<FunctionDefinitionNode>(member);
						currentFNet = currentCNet->get_function_by_name(func->get_name());

						if (func->function_is_constructor()) {
							add_in_places(currentCNet->get_state_placeM());
							add_in_places(currentFNet->get_param_place());
						}
						else {
							add_in_places(currentFNet->get_input_place());
						}

						BlockNodePtr func_body = func->get_function_body();

						translateBlock(func_body, 0);

						TransitionNodePtr end_func = std::make_shared<TransitionNode>();
						end_func->set_name(to_upper_copy(currentFNet->get_name()) + "_endf");

						ArcNodePtr oarc = generateStateOutArc();
						end_func->add_outArc(oarc);

						while (!in_places.empty()) {
							PlaceNodePtr param = in_places.back();
							std::string place_type = getPlaceTypeInFlow(param);
							ArcNodePtr arc = generateArcInByPlace(param, place_type);

							end_func->add_inArc(arc);
							in_places.pop_back();
						}
						currentFNet->add_transition(end_func);
					}
				}
				postProcessingSmartContractNet(currentCNet);

				if(i == 0)
					return currentCNet->source_code();
			}
		}
	}
    return currentCNet->source_code();
}

PlaceNodePtr Translator::translateVariableDeclaration(const ASTNodePtr& _var,const std::string& _type, const int block_num){
	if (_var != nullptr) {
		if (_var->get_node_type() == NodeTypeVariableDeclaration) {
			auto varNode = std::static_pointer_cast<VariableDeclarationNode>(_var);
			ASTNodePtr node_type = varNode->get_type();

			if (node_type != nullptr) {
				if (node_type->get_node_type() == NodeTypeMapping) {
					ListColorNodePtr mapping_type = translateMapping(_var);
					currentCNet->add_color(std::static_pointer_cast<ColorNode>(mapping_type));

					if (_type == VariableGlobalType) {
						currentCNet->add_state_color(mapping_type, varNode->get_variable_name());
					}

				}
				else if (node_type->get_node_type() == NodeTypeElementaryTypeName) {
					std::string name = std::static_pointer_cast<ElementaryTypeNameNode>(node_type)->get_type_name();
					ColorNodePtr new_type = std::make_shared<ColorNode>();
					name = to_upper_copy(name);
					new_type->set_name(name);
					new_type->set_typeDef(DataTypeRange[name]);

					std::string type_value;
					auto value = varNode->get_initial_value();
					if (value != nullptr) {
						if (value->get_node_type() == NodeTypeLiteral) {
							type_value = std::static_pointer_cast<LiteralNode>(value)->get_literal();
						}
					}
					new_type->set_init_value(type_value);
					currentCNet->add_color(new_type);

					if (_type == VariableGlobalType) {
						currentCNet->add_state_color(new_type, varNode->get_variable_name());

					}
					else if (_type == VariableLocalType) {
						PlaceNodePtr new_place = std::make_shared<PlaceNode>();
						std::string place_name = currentFNet->generatePlaceName("_" + varNode->get_variable_name());
						new_place->set_domain(name);
						new_place->set_name(place_name);

						currentFNet->add_place(new_place);
						if (block_num == 0) {
							currentFNet->add_local_variable(varNode->get_variable_name(), new_place);
						}
						else {
							tv_.push_back(generateTempVariableStruct(varNode->get_variable_name(), block_num, new_place));
						}

						return new_place;
					}
				}
				else if (node_type->get_node_type() == NodeTypeUserDefinedTypeName) {
					UserDefinedTypeNameNodePtr type = std::static_pointer_cast<UserDefinedTypeNameNode>(node_type);
					if (_type == VariableGlobalType) {
						UserDefineVariable* color = currentCNet->get_defined_color_by_name(type->get_type_name());
						if (color != NULL) {
							if (color->type == NodeTypeEnumDefinition) {
								ColorNodePtr uint_color = currentCNet->get_color_by_name("UINT");
								if (uint_color != nullptr) {
									currentCNet->add_state_color(uint_color, varNode->get_variable_name());
								}
							}
							else {
								currentCNet->add_state_color(color->color, varNode->get_variable_name());
							}
						}
						else if (get_smartcontractnet_by_name(type->get_type_name()) != nullptr) {
							SmartContractNetPtr sm_net = get_smartcontractnet_by_name(type->get_type_name());

							PlaceNodePtr sm_obj = std::make_shared<PlaceNode>();
							sm_obj->set_name(currentCNet->get_name() + "_" + varNode->get_variable_name());
							sm_obj->set_domain(sm_net->get_state_color()->get_name());

							currentCNet->add_smartcontract_obj(varNode->get_variable_name(), sm_obj, type->get_type_name());
						}
					}
					else if (_type == VariableLocalType) {
						PlaceNodePtr new_place = std::make_shared<PlaceNode>();
						std::string place_name = currentFNet->generatePlaceName("_" + varNode->get_variable_name());
						new_place->set_domain(type->get_type_name());
						new_place->set_name(place_name);

						currentFNet->add_place(new_place);
						if (block_num == 0) {
							currentFNet->add_local_variable(varNode->get_variable_name(), new_place);
						}
						else {
							tv_.push_back(generateTempVariableStruct(varNode->get_variable_name(), block_num, new_place));
						}

						return new_place;
					}
				}
				else if (node_type->get_node_type() == NodeTypeElementaryTypeName) {

				}else if (node_type->get_node_type() == NodeTypeArrayTypeName) {
					auto arrayNode = std::static_pointer_cast<ArrayTypeNameNode>(node_type);
					ListColorNodePtr array_type = translateArray(arrayNode);
					currentCNet->add_state_color(array_type, varNode->get_variable_name());
				}
			}
		}
		else if (_var->get_node_type() == NodeTypeElementaryTypeName) {
			std::string name = std::static_pointer_cast<ElementaryTypeNameNode>(_var)->get_type_name();
			ColorNodePtr new_type = std::make_shared<ColorNode>();
			name = to_upper_copy(name);
			new_type->set_name(name);
			new_type->set_typeDef(DataTypeRange[name]);
			currentCNet->add_color(new_type);

		}
		else if (_var->get_node_type() == NodeTypeArrayTypeName) {
			auto arrayNode = std::static_pointer_cast<ArrayTypeNameNode>(_var);
			ListColorNodePtr array_type = translateArray(arrayNode);
			currentCNet->add_color(array_type);
		}
	}
    return nullptr;
}

StructColorNodePtr Translator::translateStruct(const StructDefinitionNodePtr& struct_node) {
    StructColorNodePtr struct_color = std::make_shared<StructColorNode>();
    struct_color->set_name(to_upper_copy(struct_node->get_name()));
    
    for (int i = 0; i < struct_node->num_fields(); i++) {
        translateVariableDeclaration(struct_node->get_field(i),VariableNonType,0);

        VariableDeclarationNodePtr variable = std::static_pointer_cast<VariableDeclarationNode>(struct_node->get_field(i));
        ComponentNodePtr component = std::make_shared<ComponentNode>();
        component->set_name(variable->get_variable_name());

        //if type in struct does'not exist then return error
        component->set_type(to_upper_copy(std::static_pointer_cast<ElementaryTypeNameNode>(variable->get_type())->get_type_name()));//can change to function get varinfor
        struct_color->add_component(component);
    }
    return struct_color;
}

StructColorNodePtr Translator::translateEnum(const EnumDefinitionNodePtr& enum_node){
    StructColorNodePtr enum_color = std::make_shared<StructColorNode>();
    enum_color->set_name(to_upper_copy(enum_node->get_name())+"_ENUM");

    for(int i = 0; i < enum_node->num_members(); i++){
        EnumValueNodePtr enum_value = std::static_pointer_cast<EnumValueNode>(enum_node->get_member(i));

        ComponentNodePtr component = std::make_shared<ComponentNode>();
        component->set_name(enum_value->get_name());

        component->set_type("UINT");
        enum_color->add_component(component);
    }
    return enum_color;
}


ListColorNodePtr Translator::translateMapping(const ASTNodePtr& _node) {
    ListColorNodePtr list_color = std::make_shared<ListColorNode>();
    StructColorNodePtr struct_color = std::make_shared<StructColorNode>();
    
	auto var_node = std::static_pointer_cast<VariableDeclarationNode>(_node);
    auto mapNode = std::static_pointer_cast<MappingNode>(var_node->get_type());
	
	ASTNodePtr map_key = mapNode->get_key_type();
	ASTNodePtr map_value = mapNode->get_value_type();

    struct_color->set_name("STRUCT_" + to_upper_copy(var_node->get_variable_name()));
    ComponentNodePtr key = std::make_shared<ComponentNode>();
    key->set_name("adr");

	if (map_key != nullptr) {
		if (map_key->get_node_type() == NodeTypeElementaryTypeName) {
			auto key_type = std::static_pointer_cast<ElementaryTypeNameNode>(map_key);
			key->set_type(to_upper_copy(key_type->get_type_name()));
		}
		else {
			auto key_type = std::static_pointer_cast<UserDefinedTypeNameNode>(map_key);
			key->set_type(to_upper_copy(key_type->get_type_name()));
		}
		translateVariableDeclaration(map_key, VariableNonType, 0);
	}


    ComponentNodePtr value = std::make_shared<ComponentNode>();
    value->set_name("uint");

	if (map_value != nullptr) {
		if (map_value->get_node_type() == NodeTypeElementaryTypeName) {
			auto value_type = std::static_pointer_cast<ElementaryTypeNameNode>(map_value);
			value->set_type(to_upper_copy(value_type->get_type_name()));
		}
		else if (map_value->get_node_type() == NodeTypeArrayTypeName) {
			auto value_type = std::static_pointer_cast<ArrayTypeNameNode>(map_value);
			ListColorNodePtr array_type = translateArray(value_type);
			currentCNet->add_color(std::static_pointer_cast<ColorNode>(array_type));
			value->set_type(to_upper_copy(array_type->get_name()));
		}
		translateVariableDeclaration(map_value, VariableNonType, 0);
	}

    struct_color->add_component(key);
    struct_color->add_component(value);
    currentCNet->add_color(std::static_pointer_cast<ColorNode>(struct_color));
    
    list_color->set_name("LIST_"+ to_upper_copy(var_node->get_variable_name()));
    list_color->set_index_type("nat");
    list_color->set_element_type(struct_color->get_name());
    list_color->set_capacity("50");

    return list_color;
}

ListColorNodePtr Translator::translateArray(const ArrayTypeNameNodePtr& array_node){
    translateVariableDeclaration(array_node->get_base_type(),VariableNonType,0);
    auto base_type = std::static_pointer_cast<UserDefinedTypeNameNode>(array_node->get_base_type());
    ListColorNodePtr list_color = std::make_shared<ListColorNode>();
    list_color->set_name("list_"+ base_type->get_type_name());
    list_color->set_index_type("nat");
    list_color->set_element_type(base_type->get_type_name());
    list_color->set_capacity("50");
    return list_color;
}


void Translator::createFunctionInputTransition(const FunctionNetPtr& functionNet) {
	TransitionNodePtr transition = std::make_shared<TransitionNode>();
	transition->set_name(to_upper_copy(functionNet->get_name()) + "_InputD");

	PlaceNodePtr cflow = currentFNet->generateControlFlowPlace("_INPUTV");
	currentFNet->add_place(cflow);

	ArcNodePtr oarc = generateStateParOutArc(cflow);
	transition->add_outArc(oarc);

	ArcNodePtr key = std::make_shared<ArcNode>();
	key->set_placeName(currentCNet->get_func_key()->get_name());
	key->set_label("epsilon");

	transition->add_inArc(key);
	transition->add_outArc(key);

	while (!in_places.empty()) {
		PlaceNodePtr param = in_places.back();
		std::string place_type = getPlaceTypeInFlow(param);
		ArcNodePtr arc = generateArcInByPlace(param, place_type);
		transition->add_inArc(arc);

		in_places.pop_back();
	}

	functionNet->set_input_place(cflow);
	functionNet->add_transition(transition);
}


void Translator::createFunctionColorPlace(const FunctionDefinitionNodePtr& func){
    StructColorNodePtr funcParColor = std::make_shared<StructColorNode>();
    funcParColor->set_name(to_upper_copy(func->get_name())+"_PAR");

    ComponentNodePtr spcomponent = std::make_shared<ComponentNode>();
    spcomponent->set_name("sender");
    spcomponent->set_type("USER");
    funcParColor->add_component(spcomponent);

    ComponentNodePtr vpcomponent = std::make_shared<ComponentNode>();
    vpcomponent->set_name("value");
    vpcomponent->set_type("UINT");
    funcParColor->add_component(vpcomponent);

    ParameterListNodePtr func_params = func->get_params();
    for(int i = 0; i < func_params->num_parameters(); i++){
        auto param = func_params->get_parameter(i);
        if(param != nullptr && param->get_node_type() == NodeTypeVariableDeclaration){
            auto varParam = std::static_pointer_cast<VariableDeclarationNode>(param);   
			ASTNodePtr var_type = varParam->get_type();

            translateVariableDeclaration(param,VariableFunctionParmType,1);     

			if (var_type != nullptr) {
				if (var_type->get_node_type() == NodeTypeElementaryTypeName) {
					std::string type_name = std::static_pointer_cast<ElementaryTypeNameNode>(var_type)->get_type_name();

					ComponentNodePtr component = std::make_shared<ComponentNode>();
					component->set_name(varParam->get_variable_name());
					component->set_type(to_upper_copy(type_name));
					funcParColor->add_component(component);

					currentFNet->add_parameter(varParam->get_variable_name(), type_name);
				}
				else if (var_type->get_node_type() == NodeTypeArrayTypeName) {
					auto arrayNode = std::static_pointer_cast<ArrayTypeNameNode>(var_type);
					ListColorNodePtr array_type = translateArray(arrayNode);
					currentFNet->add_color(array_type);

					ComponentNodePtr component = std::make_shared<ComponentNode>();
					component->set_name(varParam->get_variable_name());
					component->set_type(to_upper_copy(array_type->get_name()));
					funcParColor->add_component(component);

					currentFNet->add_parameter(varParam->get_variable_name(), array_type->get_name());
				}
			}
        }
    }
    currentFNet->add_color(funcParColor);

    StructColorNodePtr funcStateColor = std::make_shared<StructColorNode>();
    funcStateColor->set_name(to_upper_copy(func->get_name())+"_STATE");

    ComponentNodePtr sscomponent = std::make_shared<ComponentNode>();
    sscomponent->set_name("state");
    sscomponent->set_type(currentCNet->get_state_color()->get_name());
    funcStateColor->add_component(sscomponent);

    ComponentNodePtr pscomponent = std::make_shared<ComponentNode>();
    pscomponent->set_name("par");
    pscomponent->set_type(funcParColor->get_name());
    funcStateColor->add_component(pscomponent);
    currentFNet->add_color(funcStateColor);


    PlaceNodePtr func_place = std::make_shared<PlaceNode>();
    func_place->set_name("P_"+to_upper_copy(func->get_name()));
    func_place->set_domain(funcParColor->get_name());
    currentFNet->set_param_place(func_place);

	PlaceNodePtr state_place;
	if (func->function_is_constructor()) {
		state_place = currentCNet->get_state_placeM();
	}
	else {
		state_place = currentCNet->get_state_place();
	}

    add_in_places(state_place);
    add_in_places(func_place);

	currentFNet->add_place(func_place);
	currentFNet->set_param_place(func_place);
}

void Translator::translateBlock(const BlockNodePtr& block, const int block_num){
    for(int i = 0; i < block->num_statements(); i++){
        ASTNodePtr statement = block->get_statement(i);
		if (statement != nullptr) {
			if (statement->get_node_type() == NodeTypeIfStatement) {
				IfStatementNodePtr if_statement = std::static_pointer_cast<IfStatementNode>(statement);
				translateIfStatement(if_statement, block_num);
			}
			else if (statement->get_node_type() == NodeTypeForStatement) {
				ForStatementNodePtr for_statement = std::static_pointer_cast<ForStatementNode>(statement);
				translateForStatement(for_statement, block_num);
			}
			else if (statement->get_node_type() == NodeTypeExpressionStatement) {
				ExpressionStatementNodePtr expression_statement = std::static_pointer_cast<ExpressionStatementNode>(statement);
				translateExpressionStatement(expression_statement, block_num);
			}
			else if (statement->get_node_type() == NodeTypeVariableDeclarationStatement) {
				VariableDeclarationStatementNodePtr vardecl_statement = std::static_pointer_cast<VariableDeclarationStatementNode>(statement);
				translateVariableDeclarationStatement(vardecl_statement, block_num);
			}
		}
    }
}

void Translator::translateExpressionStatement(const ExpressionStatementNodePtr& statement, const int block_num){
    translate_equivalent_node(statement->get_expression(),block_num);
}

void Translator::translate_equivalent_node(const ASTNodePtr& _node, const int block_num){
	if (_node != nullptr) {
		if (_node->get_node_type() == NodeTypeFunctionCall) {
			translateFunctionCall(std::static_pointer_cast<FunctionCallNode>(_node), block_num);
		}
		else if (_node->get_node_type() == NodeTypeVariableDeclarationStatement) {
			translateVariableDeclarationStatement(std::static_pointer_cast<VariableDeclarationStatementNode>(_node), block_num);
		}
		else if (_node->get_node_type() == NodeTypeAssignment) {
			translateAssignment(std::static_pointer_cast<AssignmentNode>(_node), block_num);
		}
	}
}

void Translator::translateFunctionCall(const FunctionCallNodePtr& func_call, const int block_num){
    if(func_call->get_callee() != nullptr){
        if(func_call->get_callee()->get_node_type() == NodeTypeIdentifier){
            IdentifierNodePtr func_name_handler = std::static_pointer_cast<IdentifierNode>(func_call->get_callee());
            if(func_name_handler->get_name() == "require"){
                translateRequireFunctionCall(func_call->get_argument(0),block_num);
			}else if (func_name_handler->get_name() == "revert") {
				translateRevertFunctionCall(func_call, block_num);
			}
        }else if(func_call->get_callee()->get_node_type() == NodeTypeMemberAccess){
            MemberAccessNodePtr member_access = std::static_pointer_cast<MemberAccessNode>(func_call->get_callee());
            std::string member = member_access->get_member();
			ASTNodePtr identifier = member_access->get_identifier();
            if(member == "transfer"){
                /* translateTransferFunctionCall(member_access->get_identifier(),func_call->get_argument(0)); */
            }else if(member == "push"){
                /* translatePushFunctionCall(member_access->get_identifier(),func_call->get_argument(0)); */
			}else if (currentCNet->get_function_by_name(member)) {

			}else {
				if (identifier != nullptr && identifier->get_node_type() == NodeTypeIdentifier) {
					IdentifierNodePtr id = std::static_pointer_cast<IdentifierNode>(identifier);
					if (currentCNet->is_smartcontract_obj(id->get_name())) {
						translateObjFunctionCall(id->get_name(), member, func_call, block_num);
					}
				}
			}
            /* translateSpecificFunctionCall(member,member_access->get_identifier(),func_call); */
        }
    }   
}

void Translator::translateObjFunctionCall(const std::string& obj_name, const std::string& func_name, const FunctionCallNodePtr& func_call, const int block_num) {
	SmartContractObject* obj = currentCNet->get_smartcontract_obj_by_name(obj_name);
	SmartContractNetPtr sm_obj = get_smartcontractnet_by_name(obj->type);
	FunctionNetPtr func_obj = sm_obj->get_function_by_name(func_name);

	TransitionNodePtr call_func = std::make_shared<TransitionNode>();
	call_func->set_name(currentFNet->generateTransitionName("_callf"));
	currentFNet->add_transition(call_func);

	PlaceNodePtr cflow1 = currentFNet->generateControlFlowPlace("_CALLF");
	currentFNet->add_place(cflow1);
	
	ArcNodePtr obj_in_arc = std::make_shared<ArcNode>();
	obj_in_arc->set_placeName(obj->place->get_name());
	std::string obj_ia_label = generateArcInLabel(obj->place, "");
	obj_in_arc->set_label("<("+obj_ia_label+")>");

	ArcNodePtr obj_funckey = std::make_shared<ArcNode>();
	obj_funckey->set_placeName(sm_obj->get_func_key()->get_name());
	obj_funckey->set_label("epsilon");
	
	ArcNodePtr obj_func_input = std::make_shared<ArcNode>();
	obj_func_input->set_placeName(func_obj->get_input_place()->get_name());

	std::vector<std::string> pc_param_label;
	std::string cpar = getParInArc();
	pc_param_label.push_back(cpar + ".sender");
	pc_param_label.push_back(cpar + ".value");

	for (size_t i = 0; i < func_call->num_arguments(); i++) {
		std::string arg = translateRequireExpressionToString(func_call->get_argument(i), block_num);
		pc_param_label.push_back(arg);
	}

	obj_func_input->set_label("<({" + obj_ia_label + ",{" + join(pc_param_label, ",") + "}})>");

	ArcNodePtr oarc_flow1 = generateStateParOutArc(cflow1);

	call_func->add_outArc(oarc_flow1);
	call_func->add_inArc(obj_in_arc);
	call_func->add_inArc(obj_funckey);
	call_func->add_outArc(obj_func_input);
	
	while (!in_places.empty()) {
		PlaceNodePtr param = in_places.back();
		std::string place_type = getPlaceTypeInFlow(param);
		ArcNodePtr arc = generateArcInByPlace(param, place_type);
		call_func->add_inArc(arc);

		in_places.pop_back();
	}
	add_in_places(cflow1);


	TransitionNodePtr call_func_e = std::make_shared<TransitionNode>();
	call_func_e->set_name(currentFNet->generateTransitionName("_callf_e"));
	currentFNet->add_transition(call_func_e);

	PlaceNodePtr cflow2 = currentFNet->generateControlFlowPlace("_CALLF_E");
	currentFNet->add_place(cflow2);

	ArcNodePtr obj_state = std::make_shared<ArcNode>();
	obj_state->set_placeName(sm_obj->get_state_place()->get_name());
	std::string sm_state_label = generateArcInLabel(sm_obj->get_state_place(), "");
	obj_state->set_label("<(" + sm_state_label + ")>");

	ArcNodePtr obj_out_arc = std::make_shared<ArcNode>();
	obj_out_arc->set_placeName(obj->place->get_name());
	obj_out_arc->set_label("<("+sm_state_label+")>");

	ArcNodePtr oarc_flow2 = generateStateParOutArc(cflow2);

	call_func_e->add_outArc(oarc_flow2);
	call_func_e->add_inArc(obj_state);
	call_func_e->add_outArc(obj_out_arc);
	call_func_e->add_outArc(obj_funckey);

	while (!in_places.empty()) {
		PlaceNodePtr param = in_places.back();
		std::string place_type = getPlaceTypeInFlow(param);
		ArcNodePtr arc = generateArcInByPlace(param, place_type);
		call_func_e->add_inArc(arc);

		in_places.pop_back();
	}
	add_in_places(cflow2);
}


void Translator::translateRequireFunctionCall(const ASTNodePtr& _arg, const int block_num){
    std::string require_exp = translateRequireExpressionToString(_arg, block_num);
    
    TransitionNodePtr revert = std::make_shared<TransitionNode>();
    revert->set_name(currentFNet->generateTransitionName("_revert"));
    revert->set_guard("not("+require_exp+");");

    TransitionNodePtr not_revert = std::make_shared<TransitionNode>();
    not_revert->set_name(currentFNet->generateTransitionName("_n_revert"));
    not_revert->set_guard(require_exp+";");

    PlaceNodePtr cflow = currentFNet->generateControlFlowPlace("_P");
    currentFNet->add_place(cflow);

    ArcNodePtr oarc = generateStateParOutArc(cflow);
    not_revert->add_outArc(oarc);
    
    ArcNodePtr narc = generateStateOutArc();
    revert->add_outArc(narc); 

    while(!in_places.empty()){
        PlaceNodePtr param = in_places.back();
        std::string place_type = getPlaceTypeInFlow(param);
        ArcNodePtr arc = generateArcInByPlace(param,place_type);
        revert->add_inArc(arc);
        not_revert->add_inArc(arc);

        in_places.pop_back();
    }

    add_in_places(cflow);

    currentFNet->add_transition(revert);
    currentFNet->add_transition(not_revert);
}

void Translator::translateRevertFunctionCall(const ASTNodePtr& _arg, const int block_num) {
	TransitionNodePtr revert = std::make_shared<TransitionNode>();
	revert->set_name(currentFNet->generateTransitionName("_e_revert"));

	ArcNodePtr oarc = std::make_shared<ArcNode>();
	oarc->set_placeName(currentCNet->get_state_place()->get_name());
	oarc->set_label("<("+getStateInArc()+")>");
	revert->add_outArc(oarc);

	while (!in_places.empty()) {
		PlaceNodePtr param = in_places.back();
		std::string place_type = getPlaceTypeInFlow(param);
		ArcNodePtr arc = generateArcInByPlace(param, place_type);
		revert->add_inArc(arc);

		in_places.pop_back();
	}
	
	currentFNet->add_transition(revert);
}


void Translator::translateVariableDeclarationStatement(const VariableDeclarationStatementNodePtr& statement, const int block_num){
    VariableDeclarationNodePtr decl = statement->get_decl();
    std::string vardec_exp = translateRequireExpressionToString(statement->get_value(), block_num);
    
    PlaceNodePtr cflow =  currentFNet->generateControlFlowPlace("_VD");
    currentFNet->add_place(cflow);

    TransitionNodePtr vardec = std::make_shared<TransitionNode>();
    vardec->set_name(currentFNet->generateTransitionName("_vd"));

    ArcNodePtr oarc = generateStateParOutArc(cflow);
    vardec->add_outArc(oarc);
    

    PlaceNodePtr color_place = translateVariableDeclaration(decl, VariableLocalType, block_num);
    ArcNodePtr decArc = std::make_shared<ArcNode>();
    std::string decArc_name;

    if(color_place != nullptr){
        decArc_name = color_place->get_name();
    }
    decArc->set_placeName(decArc_name);
    decArc->set_label("<("+vardec_exp+")>");
    vardec->add_outArc(decArc);

    while(!in_places.empty()){
        PlaceNodePtr param = in_places.back();
        std::string place_type = getPlaceTypeInFlow(param);
        ArcNodePtr arc = generateArcInByPlace(param,place_type);
        vardec->add_inArc(arc);
        
        in_places.pop_back();
    }
    add_in_places(cflow);

    currentFNet->add_transition(vardec);
}


void Translator::translateIfcondition(const IfStatementNodePtr& statement, const PlaceNodePtr& end_if, const int block_num){
    std::vector<PlaceNodePtr> init_out_places;
    
    std::string exp = translateRequireExpressionToString(statement->get_condition(), block_num);

    TransitionNodePtr ifstate = std::make_shared<TransitionNode>();
    ifstate->set_name(currentFNet->generateTransitionName("_tT"));
    ifstate->set_guard(exp+";");

    TransitionNodePtr not_ifstate = std::make_shared<TransitionNode>();
    not_ifstate->set_name(currentFNet->generateTransitionName("_tF"));
    not_ifstate->set_guard("not("+exp+");");

    PlaceNodePtr cflow_if = currentFNet->generateControlFlowPlace("_PT");
    currentFNet->add_place(cflow_if);

    ArcNodePtr oarc_if = generateStateParOutArc(cflow_if);
    ifstate->add_outArc(oarc_if);

    PlaceNodePtr cflow_not_if = currentFNet->generateControlFlowPlace("_PF");
    currentFNet->add_place(cflow_not_if);
	ArcNodePtr oarc_not_if = generateStateParOutArc(cflow_not_if);

	ArcNodePtr arc_out_not_if = generateStateParOutArc(end_if);

    while(!in_places.empty()){
        PlaceNodePtr param = in_places.back();
        std::string place_type = getPlaceTypeInFlow(param);
        ArcNodePtr arc = generateArcInByPlace(param,place_type);
        ifstate->add_inArc(arc);
        not_ifstate->add_inArc(arc);
        
        if(is_temp_variable_place(param,block_num)){
            ifstate->add_outArc(arc);
            not_ifstate->add_outArc(arc);
        }

        in_places.pop_back();
    }
    add_in_places(cflow_if);

    currentFNet->add_transition(ifstate);

    if(statement->get_then() != nullptr){
        if(statement->get_then()->get_node_type() == NodeTypeBlockNode){
            translateBlock(std::static_pointer_cast<BlockNode>(statement->get_then()), block_num+1);
        }else if(statement->get_then()->get_node_type() == NodeTypeExpressionStatement){
            translateExpressionStatement(std::static_pointer_cast<ExpressionStatementNode>(statement->get_then()), block_num+1);
        }
    }

	ArcNodePtr arc_out = generateStateParOutArc(end_if);

    TransitionNodePtr end_if_t = std::make_shared<TransitionNode>();
    end_if_t->set_name(currentFNet->generateTransitionName("_eD"));
    end_if_t->add_outArc(arc_out);

    while(!in_places.empty()){
        PlaceNodePtr param = in_places.back();
        std::string place_type = getPlaceTypeInFlow(param);
        ArcNodePtr arc = generateArcInByPlace(param,place_type);
        end_if_t->add_inArc(arc);

        in_places.pop_back();
    }
    currentFNet->add_transition(end_if_t);

    currentFNet->add_transition(not_ifstate);
    in_places.clear();
    add_in_places(cflow_not_if);

    if(statement->get_else() != nullptr){
        not_ifstate->add_outArc(oarc_not_if);

        if(statement->get_else()->get_node_type() != NodeTypeIfStatement){
            if(statement->get_else()->get_node_type() == NodeTypeExpressionStatement){
                translateExpressionStatement(std::static_pointer_cast<ExpressionStatementNode>(statement->get_else()), block_num);
            }
        }else{
            translateIfcondition(std::static_pointer_cast<IfStatementNode>(statement->get_else()), end_if, block_num);
        }
    }else{
        not_ifstate->add_outArc(arc_out_not_if);
    }
}

void Translator::translateIfStatement(const IfStatementNodePtr& statement, const int block_num){
    PlaceNodePtr end_if = currentFNet->generateControlFlowPlace("_EF");

    currentFNet->add_place(end_if);

    translateIfcondition(statement,end_if,block_num);

    in_places.clear();
    add_in_places(end_if);
}


void Translator::translateAssignment(const AssignmentNodePtr& assignment, const int block_num){
	std::string left_member = translateRequireExpressionToString(assignment->get_left_hand_operand(), block_num);
	
	if (currentCNet->is_smartcontract_obj(left_member)) {
		
		ASTNodePtr rigth_member = assignment->get_right_hand_operand();
		if (rigth_member != nullptr && rigth_member->get_node_type() == NodeTypeFunctionCall) {
			FunctionCallNodePtr fcall = std::static_pointer_cast<FunctionCallNode>(rigth_member);
			if (fcall->get_callee() != nullptr && fcall->get_callee()->get_node_type() == NodeTypeNewExpression) {
				SmartContractObject* sm_obj = currentCNet->get_smartcontract_obj_by_name(left_member);
				SmartContractNetPtr sm_net = get_smartcontractnet_by_name(sm_obj->type);

				if (sm_net->is_has_constructor()) {
					TransitionNodePtr call_constructor = std::make_shared<TransitionNode>();
					call_constructor->set_name(currentFNet->generateTransitionName("_c_cons"));
					currentFNet->add_transition(call_constructor);

					PlaceNodePtr cflow1 = currentFNet->generateControlFlowPlace("_C_CONS");
					currentFNet->add_place(cflow1);

					FunctionNetPtr fcons = currentCNet->get_constructor();
					if (fcons != nullptr) {
						PlaceNodePtr pcons = fcons->get_param_place();
						
						ArcNodePtr pc_param = std::make_shared<ArcNode>();
						pc_param->set_placeName(pcons->get_name());

						std::vector<std::string> pc_param_label;
						std::string cpar = getParInArc();
						pc_param_label.push_back(cpar + ".sender");
						pc_param_label.push_back(cpar + ".value");

						for (size_t i = 0; i < fcall->num_arguments(); i++) {
							std::string arg = translateRequireExpressionToString(fcall->get_argument(i), block_num);
							pc_param_label.push_back(arg);
						}
						
						pc_param->set_label("<({" + join(pc_param_label,",") + "})>");
						
						ArcNodePtr pc_state = std::make_shared<ArcNode>();
						pc_state->set_placeName(sm_net->get_state_placeM()->get_name());
						pc_state->set_label("<(" + createInitColor(sm_net->get_state_color(),sm_net,NodeTypeNone) + ")>");

						call_constructor->add_outArc(pc_param);
						call_constructor->add_outArc(pc_state);
						
						ArcNodePtr oarc_flow1 = generateStateParOutArc(cflow1);
						call_constructor->add_outArc(oarc_flow1);

						while (!in_places.empty()) {
							PlaceNodePtr param = in_places.back();
							std::string place_type = getPlaceTypeInFlow(param);
							ArcNodePtr arc = generateArcInByPlace(param, place_type);
							call_constructor->add_inArc(arc);

							in_places.pop_back();
						}
						add_in_places(cflow1);
					}

					TransitionNodePtr ass_obj = std::make_shared<TransitionNode>();
					ass_obj->set_name(currentFNet->generateTransitionName("_as"));
					currentFNet->add_transition(ass_obj);

					PlaceNodePtr cflow2 = currentFNet->generateControlFlowPlace("_AS");
					currentFNet->add_place(cflow2);
					
					ArcNodePtr sm_state = std::make_shared<ArcNode>();
					sm_state->set_placeName(sm_net->get_state_place()->get_name());

					std::string sm_state_label = generateArcInLabel(sm_net->get_state_place(), "");
					sm_state->set_label("<("+sm_state_label+")>");

					ArcNodePtr obj_arc = std::make_shared<ArcNode>();
					obj_arc->set_placeName(sm_obj->place->get_name());
					obj_arc->set_label("<("+sm_state_label+")>");

					ass_obj->add_outArc(obj_arc);
					ass_obj->add_inArc(sm_state);

					ArcNodePtr oarc_flow2 = generateStateParOutArc(cflow2);
					ass_obj->add_outArc(oarc_flow2);

					while (!in_places.empty()) {
						PlaceNodePtr param = in_places.back();
						std::string place_type = getPlaceTypeInFlow(param);
						ArcNodePtr arc = generateArcInByPlace(param, place_type);
						ass_obj->add_inArc(arc);

						in_places.pop_back();
					}
					add_in_places(cflow2);
				}
				else {

				}
			}
		}

	}
	else {
		std::string right_member = translateRequireExpressionToString(assignment->get_right_hand_operand(), block_num);

		TransitionNodePtr assign = std::make_shared<TransitionNode>();
		assign->set_name(currentFNet->generateTransitionName("_as"));

		PlaceNodePtr cflow = currentFNet->generateControlFlowPlace("_AS");
		currentFNet->add_place(cflow);

		std::vector<std::string> temp = split(left_member, ".");
		std::string id = temp[0];

		std::vector<std::string> cvalue = createChangeValueLet(cflow->get_name(), left_member, right_member, assignment->get_operator());
		for (auto it = cvalue.begin(); it != cvalue.end(); it++) {
			assign->add_let(*it);
		}

		ArcNodePtr oarc = std::make_shared<ArcNode>();
		oarc->set_placeName(cflow->get_name());
		if (id == "sp") {
			oarc->set_label("<(" + id + "_temp" + ")>");
		}
		else if (id == "s") {
			oarc->set_label("<({" + id + "_temp" + "," + getParInArc() + "})>");
		}
		else if (id == "p") {
			oarc->set_label("<({" + getStateInArc() + "," + id + "_temp" + "})>");
		}
		assign->add_outArc(oarc);

		while (!in_places.empty()) {
			PlaceNodePtr param = in_places.back();
			std::string place_type = getPlaceTypeInFlow(param);
			ArcNodePtr arc = generateArcInByPlace(param, place_type);
			assign->add_inArc(arc);

			in_places.pop_back();
		}
		add_in_places(cflow);

		currentFNet->add_transition(assign);
	}
}

void Translator::translateForStatement(const ForStatementNodePtr& forStatement, const int block_num){
    //init transition
    TransitionNodePtr initLoop = std::make_shared<TransitionNode>();
    initLoop->set_name(currentFNet->generateTransitionName("_t_"));

    PlaceNodePtr cflow = currentFNet->generateControlFlowPlace("_init");
    currentFNet->add_place(cflow);

    ArcNodePtr oarc_flow = generateStateParOutArc(cflow);
    initLoop->add_outArc(oarc_flow);

    while(!in_places.empty()){
        PlaceNodePtr param = in_places.back();
        std::string place_type = getPlaceTypeInFlow(param);
        ArcNodePtr arc = generateArcInByPlace(param,place_type);

        initLoop->add_inArc(arc);

        in_places.pop_back();
    }
    add_in_places(cflow);

    PlaceNodePtr initValuePlace = std::make_shared<PlaceNode>();
    ArcNodePtr valueParamInt = std::make_shared<ArcNode>();
    std::string init_value;
    std::string init_name;
    if(forStatement->get_init() != nullptr && forStatement->get_init()->get_node_type() == NodeTypeVariableDeclarationStatement){
        VariableDeclarationStatementNodePtr init_statement = std::static_pointer_cast<VariableDeclarationStatementNode>(forStatement->get_init());
        init_name = init_statement->get_decl()->get_variable_name();
        if(init_statement->get_value() != nullptr && init_statement->get_value()->get_node_type() == NodeTypeLiteral){
            LiteralNodePtr value = std::static_pointer_cast<LiteralNode>(init_statement->get_value());
            init_value = value->get_literal();
        }

        initValuePlace->set_name(currentFNet->generatePlaceName("_"+init_name));
        initValuePlace->set_domain("UINT");
        currentFNet->add_place(initValuePlace);

        valueParamInt->set_placeName(initValuePlace->get_name());
        valueParamInt->set_label("<("+init_value+")>");
        initLoop->add_outArc(valueParamInt);
    }
    tv_.push_back(generateTempVariableStruct(init_name,block_num+1,initValuePlace));
    currentFNet->add_transition(initLoop);
    
    //condition transition
    TransitionNodePtr loopCondition = std::make_shared<TransitionNode>();
    loopCondition->set_name(currentFNet->generateTransitionName("_T"));

    TransitionNodePtr loopNCondition = std::make_shared<TransitionNode>();
    loopNCondition->set_name(currentFNet->generateTransitionName("_F"));

    PlaceNodePtr cflow_s = currentFNet->generateControlFlowPlace("_c");
    currentFNet->add_place(cflow);

    PlaceNodePtr cflow_f = currentFNet->generateControlFlowPlace("_cf");
    currentFNet->add_place(cflow_f);

    ArcNodePtr oarc_flow_s = generateStateParOutArc(cflow_s);
    loopCondition->add_outArc(oarc_flow_s);

    ArcNodePtr oarc_flow_f = generateStateParOutArc(cflow_f);
    loopNCondition->add_outArc(oarc_flow_f);

    std::string exp_cond = translateRequireExpressionToString(forStatement->get_condition(),block_num+1);

    loopCondition->set_guard(exp_cond+";");
    loopNCondition->set_guard("not ("+exp_cond+");");
    
    while(!in_places.empty()){
        PlaceNodePtr param = in_places.back();
        std::string place_type = getPlaceTypeInFlow(param);
        ArcNodePtr arc = generateArcInByPlace(param,place_type);

        loopCondition->add_inArc(arc);
        loopNCondition->add_inArc(arc);

        if(place_type != PlaceStateParType){
            loopCondition->add_outArc(arc);
        }

        if(is_temp_variable_place(param,block_num+1)){
            loopNCondition->add_outArc(arc);
        }
        in_places.pop_back();
    }
    add_in_places(cflow_s);

    currentFNet->add_transition(loopCondition);
    currentFNet->add_transition(loopNCondition);
    
	if (forStatement->get_body() != nullptr) {
		if (forStatement->get_body()->get_node_type() == NodeTypeBlockNode) {
			translateBlock(std::static_pointer_cast<BlockNode>(forStatement->get_body()), block_num + 1);
		}
		else if (forStatement->get_body()->get_node_type() == NodeTypeExpressionStatement) {
			translateExpressionStatement(std::static_pointer_cast<ExpressionStatementNode>(forStatement->get_body()), block_num + 1);
		}
	}	

    //inc
    TransitionNodePtr inc = std::make_shared<TransitionNode>();
    inc->set_name(currentFNet->generateTransitionName("_inc"));

    std::string inc_value = "";
    if(forStatement->get_increment() != nullptr){
        if(forStatement->get_increment()->get_node_type() == NodeTypeExpressionStatement){
            ExpressionStatementNodePtr inc_statement = std::static_pointer_cast<ExpressionStatementNode>(forStatement->get_increment());
            std::string rinc_value = translateRequireExpressionToString(inc_statement->get_expression(), block_num+1);
            
            std::vector<std::string> trv = split(rinc_value," := ");
            if(trv.size() > 1){
                inc_value = trv[1];
            }
        }
    }

    ArcNodePtr inc_arc = std::make_shared<ArcNode>();
    inc_arc->set_placeName(initValuePlace->get_name());
    inc_arc->set_label("<("+inc_value+")>");
    inc->add_outArc(inc_arc);

    while(!in_places.empty()){
        PlaceNodePtr param = in_places.back();
        std::string place_type = getPlaceTypeInFlow(param);
        ArcNodePtr arc = generateArcInByPlace(param,place_type);

        inc->add_inArc(arc);

		inc->add_inhibitArc(arc);
		inc->add_outArc(arc);

        if(place_type == PlaceStateParType){
            ArcNodePtr rs_arc = std::make_shared<ArcNode>();
            rs_arc->set_placeName(cflow->get_name());
            rs_arc->set_label(arc->get_label());
            inc->add_outArc(rs_arc);
        }

        in_places.pop_back();
    }
    currentFNet->add_transition(inc);

}

std::string Translator::translateOperator(const std::string& _operator, const std::string& member){
    if(isNormalSign(_operator)){
        return NormalSign[_operator];
    }else{
        if(isSpecialSign(_operator)){
            if(_operator == DoublePlussSign){
                return NormalSign[EqualSign]+" "+member+" "+NormalSign[PlusSign]+" 1";
            }else if(_operator == DoubleMinusSign){
                return NormalSign[EqualSign]+" "+member+" "+NormalSign[MinusSign]+" 1";
            }else{
                std::string s(1,_operator[0]);
                return NormalSign[EqualSign]+" "+member+" "+NormalSign[s];
            }
        }
    }
    return _operator;
}

std::string Translator::translateMemberAcess(const std::string& _member){
    //temp need update in future
    if(_member == "length"){
        return "'size";
    }else if(_member == "sender"){
        return ".sender.adr";
    }
    return "."+_member;
}

FunctionNodePtr Translator::createGetMappingIndexFunction(const std::string& name, const ParamNodePtr& param1, const ParamNodePtr& param2){
    FunctionNodePtr func = std::make_shared<FunctionNode>();
    func->set_name(name);

    func->add_parameter(param1);
    func->add_parameter(param2);
    
    std::string body;
    body = body + "\tint i := 0;" +
                  "\n\twhile (i<"+param1->get_name()+"'size and "+param1->get_name()+"[i]."+param2->get_name()+"!="+param2->get_name()+") i := i+1;"+
                  "\n\tif(i<"+param1->get_name()+"'size)"+
                  "\n\t\treturn i;"+
                  "\n\telse"+
                  "\n\t\treturn -1;";
    func->set_body(body);

    func->set_returnType("int");
   return func;
}

std::vector<std::string> Translator::createChangeValueLet(const std::string& name, const std::string& left_member, const std::string& right_member, const std::string& op){
	std::vector<std::string> temp = split(left_member,".");
    std::string id = temp[0];
	std::string a_id = id;
    std::string domain;
	ColorNodePtr d_color;

    if(id == "sp"){
        domain = to_upper_copy(currentFNet->get_name()) + "_STATE";
		d_color = currentFNet->get_color_by_name(domain);
    }else if(id == "s"){
        domain = currentCNet->get_state_color()->get_name();
		d_color = std::static_pointer_cast<ColorNode>(currentCNet->get_state_color());
    }else if(id == "p"){
        domain = to_upper_copy(currentFNet->get_name()) + "_PAR";
		d_color = currentFNet->get_color_by_name(domain);
    }

	std::vector<std::string> fbody;

	if (d_color != nullptr && d_color->get_node_type() == LnaNodeTypeStruct_Color) {
		StructColorNodePtr ds_color = std::static_pointer_cast<StructColorNode>(d_color);
		for (int i = 1; i < temp.size(); i++) {
			std::string next_id = temp[i];

			ComponentNodePtr com = ds_color->get_component_by_name(next_id);
			if (com != nullptr) {
				ColorNodePtr c_color;
				if (com->get_type() == currentCNet->get_state_color()->get_name()) {
					c_color = std::static_pointer_cast<ColorNode>(currentCNet->get_state_color());
				}else{
					c_color = currentFNet->get_color_by_name(com->get_type());

					if (c_color == nullptr) {
						c_color = currentCNet->get_color_by_name(com->get_type());
					}
				}
				

				if (c_color != nullptr) {
					if (c_color->get_node_type() == LnaNodeTypeStruct_Color) {
						ds_color = std::static_pointer_cast<StructColorNode>(c_color);

						std::string let_line = domain + " " + id + "_temp " + NormalSign[EqualSign] + " " + a_id
							+ " " + AssignSign + " " +"(" + next_id + " " + NormalSign[EqualSign] + " " + next_id + "_temp" + ");";
						fbody.push_back(let_line);

						domain = com->get_type();
						id = next_id;
						a_id += "." + next_id;
					}
					else if (c_color->get_node_type() == LnaNodeTypeColor) {
						std::string let_line = domain + " " + id + "_temp " + NormalSign[EqualSign] + " " + a_id
							+ " " + AssignSign + " " + "(" + next_id + " " + NormalSign[EqualSign] + " " + com->get_type() + "(" + right_member + "));";
						fbody.push_back(let_line);

						break;
					}
				}
			}
		}
		
		std::reverse(fbody.begin(), fbody.end());
	}

	return fbody;
}

std::string Translator::translateAddressTypeValue(const std::string& _value) {
	int len = _value.size();
	int base = 1;
	int temp = 0;
	for (int i = len - 1; i >= 0; i--) {
		int ex = 0;
		if (_value[i] >= '0' && _value[i] <= '9') {
			ex = (_value[i] - 48)*base;
			base = base * 16;
		}
		else if (_value[i] >= 'A' && _value[i] <= 'F') {
			ex = (_value[i] - 55)*base;
			base = base * 16;
		}
		temp += ex;
		if (temp > 1000) {
			temp -= ex;
			break;
		}
	}
	return std::to_string(temp);
}

std::string Translator::translateRequireExpressionToString(const ASTNodePtr& _arg, const int block_num){
    std::string ret;

	if (_arg != nullptr) {
		if (_arg->get_node_type() == NodeTypeUnaryOperation) {
			UnaryOperationNodePtr exp = std::static_pointer_cast<UnaryOperationNode>(_arg);
			std::string opr = translateRequireExpressionToString(exp->get_operand(), block_num);
		}
		else if (_arg->get_node_type() == NodeTypeBinaryOperation) {
			BinaryOperationNodePtr exp = std::static_pointer_cast<BinaryOperationNode>(_arg);
			std::string left_member = translateRequireExpressionToString(exp->get_left_hand_operand(), block_num);
			std::string right_member = translateRequireExpressionToString(exp->get_right_hand_operand(), block_num);

			ret = "(" + left_member + ") " + translateOperator(exp->get_operator(), "(" + left_member + ")") + " (" + right_member + ")";
		}
		else if (_arg->get_node_type() == NodeTypeMemberAccess) {
			MemberAccessNodePtr member_access = std::static_pointer_cast<MemberAccessNode>(_arg);
			std::string identifier = translateRequireExpressionToString(member_access->get_identifier(), block_num);
			std::string member = translateMemberAcess(member_access->get_member());
			ret = identifier + member;
			

		}
		else if (_arg->get_node_type() == NodeTypeIdentifier) {
			std::string operand;
			operand = std::static_pointer_cast<IdentifierNode>(_arg)->get_name();
			if (isInteger(operand)) {
				ret = operand;
			}
			else if (operand == "msg") {
				ret = getParInArc();
			}
			else if (currentFNet->is_local_variable(operand)) {
				PlaceNodePtr lc_place = currentFNet->get_local_variable_by_name(operand);
				add_in_places(lc_place);

				ret = generateArcInLabel(lc_place, PlaceVarialeLocalType);
			}
			else if (currentCNet->is_state_color(operand)) {
				ret = getStateInArc() + "." + operand;
			}
			else if (is_temp_variable(operand, block_num)) {
				temp_variable* tv = get_temp_variable(operand, block_num);
				add_in_places(tv->place);

				ret = generateArcInLabel(tv->place, PlaceVarialeLocalType);
			}
			else if (currentFNet->is_parameter(operand)) {
				ret = getParInArc() + "." + operand;
			}
			else {
				ret = operand;
			}
		}
		else if (_arg->get_node_type() == NodeTypeLiteral) {
			std::string literal = std::static_pointer_cast<LiteralNode>(_arg)->get_literal();

			if (literal.size() > 2 && literal.substr(0, 2) == "0x") {
				ret = translateAddressTypeValue(literal.substr(2, literal.size() - 2));
			}
			else {
				ret = literal;
			}
		}
		else if (_arg->get_node_type() == NodeTypeIndexAccess) {
			IndexAccessNodePtr index = std::static_pointer_cast<IndexAccessNode>(_arg);
			std::string identifier = translateRequireExpressionToString(index->get_identifier(), block_num);
			std::string index_value = translateRequireExpressionToString(index->get_index_value(), block_num);
			std::vector<std::string> temp_id = split(identifier, ".");
			std::vector<std::string> temp_in = split(index_value, ".");
			std::string identifier_last = temp_id[temp_id.size() - 1];
			std::string index_last = temp_in[temp_in.size() - 1];

			if (isInteger(index_value) ||
				(currentCNet->is_state_color(index_last) && currentCNet->get_state_color_by_name(index_last)->get_name() == "UINT8")) {
				ret = identifier + "[nat(" + index_value + ")]";
			}
			else {
				if (currentCNet->is_state_color(identifier_last)) {
					ColorNodePtr or_color = currentCNet->get_state_color_by_name(identifier_last);
					if (or_color != nullptr && or_color->get_node_type() == LnaNodeTypeListColor) {
						ListColorNodePtr color = std::static_pointer_cast<ListColorNode>(or_color);
						ColorNodePtr color_type = currentCNet->get_color_by_name(color->get_element_type());

						if (color_type != nullptr && color_type->get_node_type() == LnaNodeTypeStruct_Color) {
							StructColorNodePtr color_type_struct = std::static_pointer_cast<StructColorNode>(color_type);
							std::string func_name;
							func_name = "get" + color->get_name() + "Index";

							ParamNodePtr param1 = std::make_shared<ParamNode>();
							ParamNodePtr param2 = std::make_shared<ParamNode>();
							param1->set_name("gis");
							param1->set_type(color->get_name());
							param2->set_name(color_type_struct->get_component(0)->get_name());
							param2->set_type(color_type_struct->get_component(0)->get_type());

							FunctionNodePtr mif_function = createGetMappingIndexFunction(func_name, param1, param2);
							currentCNet->add_function_node(mif_function);

							ret = identifier + "[" + func_name + "(" + identifier + "," + index_value + ")]." + color_type_struct->get_component(1)->get_name();
						}
					}
				}
			}
		}
	}
    
    return ret;
}


/** Process
 * 
 */

std::string Translator::getInStateType() {
	for (auto it = in_places.begin(); it != in_places.end(); it++) {
		if ((*it)->get_domain() == to_upper_copy(currentFNet->get_name()) + "_STATE") {
			return PlaceStateParType;
		}
		
		if ((*it)->get_domain() == currentCNet->get_state_color()->get_name()) {
			return PlaceStateType;
		}
	}
	return PlaceNoneType;
}

std::string Translator::getStateInArc(){
	std::string type = getInStateType();
	if (type == PlaceStateType) {
		return "s";
	}
	else if (type == PlaceStateParType) {
		return "sp.state";
	}
	else if (type == PlaceNoneType) {
		return "epsilon";
	}
	return "";
}

std::string Translator::getParInArc(){
	std::string type = getInStateType();
    if(type == PlaceStateType){
        return "p";
    }else if(type == PlaceStateParType){
        return "sp.par";
	}
	else if (type == PlaceNoneType) {
		return "epsilon";
	}
    return "";
}

ArcNodePtr Translator::generateStateOutArc(){
    ArcNodePtr narc = std::make_shared<ArcNode>();
    narc->set_placeName(currentCNet->get_state_place()->get_name());
    narc->set_label("<("+getStateInArc()+")>");
    return narc;
}

ArcNodePtr Translator::generateStateParOutArc(const PlaceNodePtr& _place){
    ArcNodePtr narc = std::make_shared<ArcNode>();
    narc->set_placeName(_place->get_name());
    std::string arc_p;

	std::string type = getInStateType();
    if(type == PlaceStateType){
        arc_p = "{" + getStateInArc() + "," + getParInArc() + "}";
    }else if(type == PlaceStateParType){
        arc_p = "sp";
	}
	else if (type == PlaceNoneType) {
		arc_p = "epsilon";
	}

    narc->set_label("<("+arc_p+")>");
    return narc;
}

std::string Translator::getPlaceTypeInFlow(const PlaceNodePtr _place){
    std::string domain = _place->get_domain();
	
    if(domain == currentCNet->get_state_color()->get_name()){
        return PlaceStateType;
    }else if(domain == to_upper_copy(currentFNet->get_name()) + "_STATE"){
        return PlaceStateParType;
    }else if(domain == to_upper_copy(currentFNet->get_name()) + "_PAR"){
        return PlaceParType;
    }else{
        std::vector<std::string> temp = split_ex(_place->get_name(),"_",2);
        std::string place_name = temp.back();

		if (place_name.size() > 0) {
			place_name.pop_back();
		}

        if(currentFNet->is_local_variable(place_name)){
            return PlaceVarialeLocalType;
        }
    }

    return "";
}

std::string Translator::generateArcInLabel(const PlaceNodePtr& _place, const std::string& _place_type){
    std::string arc_p; 
    if(_place_type == PlaceStateType){
        arc_p = "s";
    }else if(_place_type == PlaceStateParType){
        arc_p = "sp";
    }else if(_place_type == PlaceParType){
        arc_p = "p";
    }else{
        std::vector<std::string> temp = split_ex(_place->get_name(),"_",2);
        arc_p = temp.back();
    }

    return arc_p;
}

ArcNodePtr Translator::generateArcInByPlace(const PlaceNodePtr& _place,const std::string& _place_type){
    std::string arc_p = generateArcInLabel(_place, _place_type);

    ArcNodePtr arc = std::make_shared<ArcNode>();
    arc->set_placeName(_place->get_name());
    arc->set_label("<("+arc_p+")>");
    return arc;
}

temp_variable* Translator::generateTempVariableStruct(const std::string& name, const int& block_num, const PlaceNodePtr& place){
    temp_variable *tv_ = new temp_variable();
    tv_->name = name;
    tv_->block_num = block_num;
    tv_->place = place;

    return tv_;
}

temp_variable* Translator::get_temp_variable(const std::string& name, const int block_num){
    for(int i = tv_.size() - 1; i >= 0; i--){
        if(tv_[i]->name == name && tv_[i]->block_num <= block_num){
            return tv_[i];
        }
    }

    return NULL;
}

bool Translator::is_temp_variable(const std::string& name, const int block_num){
    for(int i = tv_.size() - 1; i >= 0; i--){
        if(tv_[i]->name == name && tv_[i]->block_num <= block_num){
            return true;
        }
    }

    return false;
}

bool Translator::is_temp_variable_place(const PlaceNodePtr& place, const int block_num){
    for(int i = tv_.size() - 1; i >= 0; i--){
        if(tv_[i]->place->get_name() == place->get_name() && tv_[i]->block_num <= block_num){
            return true;
        }
    }

    return false;
}

void Translator::add_in_places(const PlaceNodePtr& place){
    for(auto it = in_places.begin(); it != in_places.end(); it++){
        if((*it)->get_name() == place->get_name()){
            return;
        }
    }

    in_places.push_back(place);
}


/*** post-processing
*/
void Translator::postProcessingSmartContractNet(const SmartContractNetPtr& smartcontractNet) {
	StructColorNodePtr state_color = smartcontractNet->get_state_color();
	PlaceNodePtr state_place = smartcontractNet->get_state_place();
	PlaceNodePtr state_placeM = smartcontractNet->get_state_placeM();

	std::string place_init = "<(" + createInitColor(state_color, smartcontractNet, NodeTypeNone) + ")>";
	
	if (smartcontractNet->is_has_constructor()) {
		state_placeM->set_init(place_init);
	}
	else {
		state_place->set_init(place_init);
	}
}

std::string Translator::createInitColor(const ColorNodePtr& _color, const SmartContractNetPtr& smartcontractNet, NodeType type) {
	std::string ret;

	if (type == NodeTypeEnumDefinition) {
		StructColorNodePtr struct_color = std::static_pointer_cast<StructColorNode>(_color);
		std::vector<std::string> list_c;
		for (size_t i = 0; i < struct_color->num_components(); i++) {
			list_c.push_back(std::to_string(i));
		}
		ret = "{" + join(list_c, ",") + "}";
	}else if (type == NodeTypeStructDefinition) {
		ListColorNodePtr list_color = std::static_pointer_cast<ListColorNode>(_color);

	}else {
		if (_color != nullptr) {
			if (_color->get_node_type() == LnaNodeTypeStruct_Color) {
				StructColorNodePtr struct_color = std::static_pointer_cast<StructColorNode>(_color);
				std::vector<std::string> list_c;

				for (size_t i = 0; i < struct_color->num_components(); i++) {
					ComponentNodePtr com = struct_color->get_component(i);

					UserDefineVariable* uv = smartcontractNet->get_defined_color_by_name(com->get_name());
					if (uv != NULL) {
						list_c.push_back(createInitColor(uv->color, smartcontractNet, uv->type));
					}
					else {
						ColorNodePtr color = smartcontractNet->get_color_by_name(com->get_type());
						list_c.push_back(createInitColor(color, smartcontractNet, NodeTypeNone));
					}
				}

				ret = "{" + join(list_c, ",") + "}";
			}
			else if (_color->get_node_type() == LnaNodeTypeColor) {
				ret = "0";
			}
			else {
				ret = "0";
			}
		}
	}
	return ret;
}


}


