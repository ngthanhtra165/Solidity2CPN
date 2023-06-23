#ifndef SOL2CPN_JCAANALYSER_H_
#define SOL2CPN_JCAANALYSER_H_

#include "../include/nlohmann/json.hpp"
#include "../include/Utils.hpp"
#include "ASTNodes.hpp"

namespace SOL2CPN {

class JCAAnalyser {
public:
	JCAAnalyser(nlohmann::json& _jca_json);
	RootNodePtr analyse();

private:
	nlohmann::json jca_json;
	std::vector<ContractDefinitionNodePtr> contracts;
	ContractDefinitionNodePtr current_contract;
	std::string current_contract_name;
	unsigned int num_functions_current_contract;

	void analyseContract(nlohmann::json& contract_json);

	BlockNodePtr handle_block(nlohmann::json& block_json);//
	ExpressionStatementNodePtr handle_expression_statament(nlohmann::json& expression_json);//
	ASTNodePtr get_value_equivalent_node(std::string& token, nlohmann::json& token_json);//
	ASTNodePtr get_statement_equivalent_node(std::string& token, nlohmann::json& token_json);//
	AssignmentNodePtr handle_assignment(nlohmann::json& assignment_json);//
	EnumDefinitionNodePtr handle_enum_definition(nlohmann::json& enum_json);//
	EnumValueNodePtr handle_enum_value(nlohmann::json& enum_value_json);//
	IfStatementNodePtr handle_if_statement(nlohmann::json& if_statement_json);//
	VariableDeclarationNodePtr handle_variable_declaration(nlohmann::json& var_json);//
	ASTNodePtr get_type_name(nlohmann::json& type_name_json);//
	ParameterListNodePtr handle_parameter_list(nlohmann::json& param_list_json);//
	BinaryOperationNodePtr handle_binary_operation(nlohmann::json& binary_op_json);//
	UnaryOperationNodePtr handle_unary_operation(nlohmann::json& unary_op_json);//
	LiteralNodePtr handle_literal(nlohmann::json& literal_json);//
	TupleExpressionNodePtr handle_tuple_expression(nlohmann::json& tuple_exp_json);//
	VariableDeclarationStatementNodePtr handle_variable_declaration_statament(nlohmann::json& var_decl_json);//
	IdentifierNodePtr handle_identifier(nlohmann::json& identifier_json);//
	ReturnNodePtr handle_return(nlohmann::json& return_json);//
	FunctionCallNodePtr handle_function_call(nlohmann::json& function_call_json);//
	MemberAccessNodePtr handle_member_access(nlohmann::json& member_access_json);//
	EmitStatementNodePtr handle_emit_statement(nlohmann::json& emit_statement_json);//
	IndexAccessNodePtr handle_index_access(nlohmann::json& index_json);//
	ElementaryTypeNameExpressionNodePtr handle_elementary_type_name_expression(nlohmann::json& ele_type_name_exp_json);//
	ConditionalNodePtr handle_conditional(nlohmann::json& conditional_json);//
	ForStatementNodePtr handle_for_statement(nlohmann::json& for_statement_json);//
	DoWhileStatementNodePtr handle_do_while_statament(nlohmann::json& do_while_json);
	WhileStatementNodePtr handle_while_statement(nlohmann::json& while_statement_json);//
	ASTNodePtr get_unknown(std::string& token, nlohmann::json& unknown_json);//
	NewExpresionNodePtr handle_new_expression(nlohmann::json& new_exp_json);//
	PlaceHolderStatementPtr handle_placeholder(nlohmann::json& placeholder_json);//
	ModifierInvocationNodePtr handle_modifier_invocation(nlohmann::json& modifier_invocation_json);
};

}


#endif