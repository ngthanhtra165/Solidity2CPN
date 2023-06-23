#include "JCAAnalyser.hpp"

namespace SOL2CPN {

JCAAnalyser::JCAAnalyser(nlohmann::json& _jca_json) {
	this->jca_json = _jca_json;
}


RootNodePtr JCAAnalyser::analyse() {
	auto root_node = this->jca_json.at("nodes");

	for (size_t i = 0; i < root_node.size(); i++) {
		auto node = root_node.at(i);

		if (node.contains("nodeType")) {
			std::string  keyword = node.at("nodeType");
			if (keyword == TokenContractDefinition) {
				std::string contract_name = node.at("name");
				ContractDefinitionNodePtr contract(new ContractDefinitionNode(contract_name));
				std::string kind = node.at("contractKind");
				contract->set_contract_kind(kind);
				this->contracts.push_back(contract);
				this->current_contract = contract;
				this->current_contract_name = contract_name;
				this->num_functions_current_contract = 0;

				analyseContract(node);
			}
			else if (keyword == TokenImportDirective) {
				std::string import_node_name = node.at("absolutePath");
				import_node_name = split(import_node_name, ".")[0];
				ImportDirectiveNodePtr import_node = std::make_shared<ImportDirectiveNode>();
				import_node->set_import_node_name(import_node_name);
			}
		}
	}

	RootNodePtr ast_root = std::make_shared<RootNode>();
	std::string node_name = this->jca_json.at("absolutePath");
	node_name = split(node_name, ".")[0];
	ast_root->set_name(node_name);

	for (auto it_contract = contracts.begin(); it_contract != contracts.end(); ++it_contract) {
		ast_root->add_field(*it_contract);
	}

	return ast_root;
}

void JCAAnalyser::analyseContract(nlohmann::json& contract_json) {
	auto contract_nodes = contract_json.at("nodes");

	for (size_t i = 0; i < contract_nodes.size(); i++) {
		auto node = contract_nodes.at(i);

		if (node.contains("nodeType")) {
			std::string keyword = node.at("nodeType");

			if (keyword == TokenEnumDefinition) {
				EnumDefinitionNodePtr enum_def = handle_enum_definition(node);
				current_contract->add_member(enum_def);
			}
			else if (keyword == TokenVariableDeclaration) {
				VariableDeclarationNodePtr var_decl = handle_variable_declaration(node);
				current_contract->add_member(var_decl);
			}
			else if (keyword == TokenStructDefinition) {
				std::string struct_name = node.at("name");
				StructDefinitionNodePtr struct_node = std::make_shared<StructDefinitionNode>(struct_name);

				auto members = node.at("members");

				for (size_t z = 0; z < members.size(); z++) {
					auto member = members.at(z);

					if (member.contains("nodeType")) {
						std::string token = member.at("nodeType");
						if (token == TokenVariableDeclaration) {
							auto ast_var_decl = std::static_pointer_cast<ASTNode>(handle_variable_declaration(member));
							struct_node->add_field(ast_var_decl);
						}
					}
				}
				auto ast_struct_node = std::static_pointer_cast<ASTNode>(struct_node);
				current_contract->add_member(ast_struct_node);
			}
			else if (keyword == TokenFunctionDefinition) {
				FunctionDefinitionNodePtr function_node = std::make_shared<FunctionDefinitionNode>();
				std::string function_name = node.at("name");
				function_node->set_name(function_name);


				std::string qualifier = node.at("visibility");
				std::string stateMutability = node.at("stateMutability");

				if (stateMutability != "nonpayable") {
					qualifier = qualifier + " " + stateMutability;
				}
				function_node->set_qualifier(qualifier);

				std::string kind = node.at("kind");
				bool is_constructor = (kind == "constructor");
				function_node->set_is_constructor(is_constructor);

				ParameterListNodePtr params = handle_parameter_list(node.at("parameters"));
				function_node->set_params(params);

				ParameterListNodePtr returns = handle_parameter_list(node.at("returnParameters"));
				function_node->set_returns(returns);

				/*
				ModifierInvocationNodePtr modifier_invocation = nullptr;
				while (token == TokenModifierInvocation) {
					modifier_invocation = handle_modifier_invocation();
					function_node->add_modifier_invocation(modifier_invocation);
					token = get_next_token();
				}
				*/

				BlockNodePtr function_body = nullptr;
				auto body = node.at("body");
				if (body.contains("nodeType")) {
					std::string token = body.at("nodeType");

					if (token == TokenBlock) {
						function_body = handle_block(body.at("statements"));
					}
					function_node->set_function_body(function_body);
				}

				current_contract->add_member(function_node);

				num_functions_current_contract++;
			}
		}
		
	}
}

BlockNodePtr JCAAnalyser::handle_block(nlohmann::json& block_json) {
	BlockNodePtr block = std::make_shared<BlockNode>();
	for (size_t i = 0; i < block_json.size(); i++) {
		auto node = block_json.at(i);

		if (node.contains("nodeType")) {
			std::string token = node.at("nodeType");

			if (token == TokenExpressionStatement) {
				ExpressionStatementNodePtr expression = handle_expression_statament(node.at("expression"));
				block->add_statement(expression);
			}
			else if (token == TokenEmitStatement) {
				EmitStatementNodePtr emit_statement = handle_emit_statement(node);
				block->add_statement(emit_statement);
			}
			else if (token == TokenIfStatement) {
				IfStatementNodePtr if_statement = handle_if_statement(node);
				block->add_statement(if_statement);
			}
			else if (token == TokenForStatement) {
				ForStatementNodePtr for_statement = handle_for_statement(node);
				block->add_statement(for_statement);
			}
			else if (token == TokenWhileStatement) {
				WhileStatementNodePtr while_statement = handle_while_statement(node);
				block->add_statement(while_statement);
			}
			else if (token == TokenDoWhileStatement) {

			}
			else if (token == TokenVariableDeclarationStatement) {
				VariableDeclarationStatementNodePtr var_decl_statement = handle_variable_declaration_statament(node);
				block->add_statement(var_decl_statement);
			}
			else if (token == TokenReturn) {
				ReturnNodePtr return_statement = handle_return(node);
				block->add_statement(return_statement);
			}
		}
	}
	return block;
}

ForStatementNodePtr JCAAnalyser::handle_for_statement(nlohmann::json& for_statement_json) {
	auto condition_json = for_statement_json.at("condition");
	auto init_exp_json = for_statement_json.at("initializationExpression");
	auto loop_exp_json = for_statement_json.at("loopExpression");
	auto body_json = for_statement_json.at("body");

	ASTNodePtr condition = nullptr;
	ASTNodePtr init_exp = nullptr;
	ASTNodePtr loop_exp = nullptr;
	ASTNodePtr body = nullptr;

	std::string token;

	if (condition_json.contains("nodeType")) {
		token = condition_json.at("nodeType");
		condition = get_unknown(token, condition_json);
	}

	if (init_exp_json.contains("nodeType")) {
		token = init_exp_json.at("nodeType");
		init_exp = get_unknown(token, init_exp_json);
	}
	
	if (loop_exp_json.contains("nodeType")) {
		token = loop_exp_json.at("nodeType");
		loop_exp = get_unknown(token, loop_exp_json);
	}
	
	if (body_json.contains("nodeType")) {
		token = body_json.at("nodeType");
		body = get_statement_equivalent_node(token, body_json);
	}
	
	return std::make_shared<ForStatementNode>(init_exp, condition, loop_exp, body);
}


IfStatementNodePtr JCAAnalyser::handle_if_statement(nlohmann::json& if_statement_json) {
	auto condition_json = if_statement_json.at("condition");
	auto truebody_json = if_statement_json.at("trueBody");
	std::string token;

	ASTNodePtr condition;
	ASTNodePtr if_then = nullptr;
	ASTNodePtr if_else = nullptr;

	if (condition_json.contains("nodeType")) {
		token = condition_json.at("nodeType");
		condition = get_value_equivalent_node(token, condition_json);
	}
	
	if (truebody_json.contains("nodeType")) {
		token = truebody_json.at("nodeType");
		if_then = get_statement_equivalent_node(token, truebody_json);
	}

	auto falsebody_json = if_statement_json.at("falseBody");
	if (falsebody_json.contains("nodeType")) {
		token = falsebody_json.at("nodeType");
		if_else = get_statement_equivalent_node(token, falsebody_json);
	}

	IfStatementNodePtr if_stmt = std::make_shared<IfStatementNode>(condition, if_then, if_else);
	return if_stmt;
}


ExpressionStatementNodePtr JCAAnalyser::handle_expression_statament(nlohmann::json& expression_json) {
	ExpressionStatementNodePtr statement = std::make_shared<ExpressionStatementNode>();
	ASTNodePtr expression = nullptr;
	
	if (expression_json.contains("nodeType")) {
		std::string token = expression_json.at("nodeType");
		expression = get_value_equivalent_node(token, expression_json);
	}

	statement->set_expression(expression);
	return statement;
}

ASTNodePtr JCAAnalyser::get_statement_equivalent_node(std::string& token, nlohmann::json& token_json) {
	if (token == TokenBlock) {
		return handle_block(token_json.at("statements"));
	}
	else if (token == TokenPlaceholderStatement) {
		return handle_placeholder(token_json);
	}
	else if (token == TokenIfStatement) {
		return handle_if_statement(token_json);
	}
	else if (token == TokenDoWhileStatement) {
		//return handle_do_while_statament();
	}
	else if (token == TokenWhileStatement) {
		return handle_while_statement(token_json);
	}
	else if (token == TokenForStatement) {
		return handle_for_statement(token_json);
	}
	else if (token == TokenEmitStatement) {
		return handle_emit_statement(token_json);
	}
	else if (token == TokenVariableDeclarationStatement) {
		return handle_variable_declaration_statament(token_json);
	}
	else if (token == TokenExpressionStatement) {
		return handle_expression_statament(token_json.at("expression"));
	}
	else if (token == TokenBreak) {
		return std::make_shared<BreakNode>();
	}
	else if (token == TokenContinue) {
		return std::make_shared<ContinueNode>();
	}
	else if (token == TokenReturn) {
		return handle_return(token_json);
	}
	else if (token == TokenThrow) {
		return std::make_shared<ThrowNode>();
	}
	else if (token == TokenInlineAssembly) {
		
	}
	return nullptr;
}


PlaceHolderStatementPtr JCAAnalyser::handle_placeholder(nlohmann::json& placeholder_json) {
	std::string placeholder = "";
	return std::make_shared<PlaceHolderStatement>(placeholder);
}

EmitStatementNodePtr JCAAnalyser::handle_emit_statement(nlohmann::json& emit_statement_json) {
	auto event_call_json = emit_statement_json.at("eventCall");

	ASTNodePtr thing_to_emit = nullptr;

	if (event_call_json.contains("nodeType")) {
		std::string token = event_call_json.at("nodeType");
		thing_to_emit = get_value_equivalent_node(token, event_call_json);
	}
	
	EmitStatementNodePtr emit_stmt = std::make_shared<EmitStatementNode>();
	emit_stmt->set_event_call(thing_to_emit);
	return emit_stmt;
}


ASTNodePtr JCAAnalyser::get_unknown(std::string& token, nlohmann::json& unknown_json) {
	if (std::find(StatementTokenList.begin(), StatementTokenList.end(), token) != StatementTokenList.end()) {
		return get_statement_equivalent_node(token, unknown_json);
	}
	else if (std::find(ExpressionTokenList.begin(), ExpressionTokenList.end(), token) != ExpressionTokenList.end()) {
		return get_value_equivalent_node(token, unknown_json);
	}
	return nullptr;
}

WhileStatementNodePtr JCAAnalyser::handle_while_statement(nlohmann::json& while_statement_json) {
	auto body_json = while_statement_json.at("body");
	auto condition_json = while_statement_json.at("condition");

	ASTNodePtr condition = nullptr;
	ASTNodePtr loop_body = nullptr;

	std::string token;

	if (condition_json.contains("nodeType")) {
		std::string token = condition_json.at("nodeType");
		condition = get_unknown(token, condition_json);
	}

	if (condition_json.contains("nodeType")) {
		token = body_json.at("nodeType");
		loop_body = get_unknown(token, body_json);
	}

	WhileStatementNodePtr while_do = std::make_shared<WhileStatementNode>(condition, loop_body);
	return while_do;
}


ReturnNodePtr JCAAnalyser::handle_return(nlohmann::json& return_json) {
	auto expression_json = return_json.at("expression");

	ReturnNodePtr return_node = std::make_shared<ReturnNode>();
	ASTNodePtr sub_node = nullptr;
	if (expression_json.contains("nodeType")) {
		std::string token = expression_json.at("nodeType");
		sub_node = get_value_equivalent_node(token, expression_json);
	}
	return_node->set_operand(sub_node);

	return return_node;
}


VariableDeclarationStatementNodePtr JCAAnalyser::handle_variable_declaration_statament(nlohmann::json& var_decl_json) {
	auto assignments_json = var_decl_json.at("assignments");
	auto declarations_json = var_decl_json.at("declarations");
	auto init_value_json = var_decl_json.at("initialValue");
	
	VariableDeclarationNodePtr decl;
	for (size_t i = 0; i < declarations_json.size(); i++) {
		auto decl_json = declarations_json.at(i);
		decl = handle_variable_declaration(decl_json);
	}

	VariableDeclarationStatementNodePtr variable_decl_stmt;
	if (init_value_json.contains("nodeType")) {
		std::string token = init_value_json.at("nodeType");
		ASTNodePtr value = get_value_equivalent_node(token, init_value_json);
		variable_decl_stmt = std::make_shared<VariableDeclarationStatementNode>(decl, value);
	}
	else {
		ASTNodePtr value = nullptr;
		variable_decl_stmt = std::make_shared<VariableDeclarationStatementNode>(decl, value);
	}

	return variable_decl_stmt;
}

ASTNodePtr JCAAnalyser::get_value_equivalent_node(std::string& token, nlohmann::json& token_json) {
	ASTNodePtr node;

	if (token == TokenBinaryOperation) {
		node = handle_binary_operation(token_json);
	}
	else if (token == TokenLiteral) {
		node = handle_literal(token_json);
	}
	else if (token == TokenTupleExpression) {
		node = handle_tuple_expression(token_json);
	}
	else if (token == TokenUnaryOperation) {
		node = handle_unary_operation(token_json);
	}
	else if (token == TokenIdentifier) {
		node = handle_identifier(token_json);
	}
	else if (token == TokenFunctionCall) {
		node = handle_function_call(token_json);
	}
	else if (token == TokenMemberAccess) {
		node = handle_member_access(token_json);
	}
	else if (token == TokenIndexAccess) {
		node = handle_index_access(token_json);
	}
	else if (token == TokenElementaryTypeNameExpression) {
		node = handle_elementary_type_name_expression(token_json);
	}
	else if (token == TokenConditional) {
		node = handle_conditional(token_json);
	}
	else if (token == TokenAssignment) {
		node = handle_assignment(token_json);
	}
	else if (token == TokenNewExpression) {
		node = handle_new_expression(token_json);
	}
	return node;
}


ConditionalNodePtr JCAAnalyser::handle_conditional(nlohmann::json& conditional_json) {
	auto condition_json = conditional_json.at("condition");
	auto false_exp_json = conditional_json.at("falseExpression");
	auto true_exp_json = conditional_json.at("trueExpression");

	ASTNodePtr condition = nullptr;
	ASTNodePtr yes = nullptr;
	ASTNodePtr no = nullptr;

	std::string token;

	if (condition_json.contains("nodeType")) {
		token = condition_json.at("nodeType");
		condition = get_value_equivalent_node(token, condition_json);
	}
	
	if (true_exp_json.contains("nodeType")) {
		token = true_exp_json.at("nodeType");
		yes = get_value_equivalent_node(token, true_exp_json);
	}
	
	if (false_exp_json.contains("nodeType")) {
		token = false_exp_json.at("nodeType");
		no = get_value_equivalent_node(token, false_exp_json);
	}

	ConditionalNodePtr condiitonal = std::make_shared<ConditionalNode>(condition, yes, no);
	return condiitonal;
}


NewExpresionNodePtr JCAAnalyser::handle_new_expression(nlohmann::json& new_exp_json) {
	auto  type_name_json = new_exp_json.at("typeName");
	ASTNodePtr type_name = get_type_name(type_name_json);
	return std::make_shared<NewExpresionNode>(type_name);
}


ElementaryTypeNameExpressionNodePtr JCAAnalyser::handle_elementary_type_name_expression(nlohmann::json& ele_type_name_exp_json) {
	auto type_name = ele_type_name_exp_json.at("typeName");
	std::string name;
	
	if (type_name.contains("nodeType")) {
		std::string token = type_name.at("nodeType");
		if (token == TokenElementaryTypeName) {
			name = type_name.at("name");
		}
	}
	else {
		name = type_name;
	}
	
	ElementaryTypeNameExpressionNodePtr elementary_type_expr = std::make_shared<ElementaryTypeNameExpressionNode>(name);
	return elementary_type_expr;
}


IndexAccessNodePtr JCAAnalyser::handle_index_access(nlohmann::json& index_json) {
	auto base_exp_json = index_json.at("baseExpression");
	auto index_exp_json = index_json.at("indexExpression");

	ASTNodePtr identifier = nullptr;
	ASTNodePtr index = nullptr;
	std::string token;

	if (base_exp_json.contains("nodeType")) {
		token = base_exp_json.at("nodeType");
		identifier = get_value_equivalent_node(token, base_exp_json);
	}
	
	if (index_exp_json.contains("nodeType")) {
		token = index_exp_json.at("nodeType");
		index = get_value_equivalent_node(token, index_exp_json);
	}
	
	IndexAccessNodePtr index_access = std::make_shared<IndexAccessNode>();
	index_access->set_identifier(identifier);
	index_access->set_index_value(index);

	return index_access;
}


TupleExpressionNodePtr JCAAnalyser::handle_tuple_expression(nlohmann::json& tuple_exp_json) {
	TupleExpressionNodePtr tuple_expression = std::make_shared<TupleExpressionNode>();
	
	auto components_json = tuple_exp_json.at("components");

	for (size_t i = 0; i < components_json.size(); i++) {
		auto component = components_json.at(i);

		if (component.contains("nodeType")) {
			std::string token = component.at("nodeType");
			ASTNodePtr node = get_value_equivalent_node(token, component);
			tuple_expression->add_member(node);
		}
	}

	return tuple_expression;
}

UnaryOperationNodePtr JCAAnalyser::handle_unary_operation(nlohmann::json& unary_op_json) {
	std::string op = unary_op_json.at("operator");
	bool is_prefix = unary_op_json.at("prefix");
	
	ASTNodePtr operand = nullptr;

	auto sub_exp_json = unary_op_json.at("subExpression");
	if (sub_exp_json.contains("nodeType")) {
		std::string token = sub_exp_json.at("nodeType");
		operand = get_value_equivalent_node(token, sub_exp_json);
	}
	
	UnaryOperationNodePtr unary_operation = std::make_shared<UnaryOperationNode>(op, operand, is_prefix);
	return unary_operation;
}


MemberAccessNodePtr JCAAnalyser::handle_member_access(nlohmann::json& member_access_json) {
	std::string member = member_access_json.at("memberName");
	auto expression = member_access_json.at("expression");

	ASTNodePtr identifier = nullptr;
	if (expression.contains("nodeType")) {
		std::string token = expression.at("nodeType");
		identifier = get_value_equivalent_node(token, expression);
	}
	
	return std::make_shared<MemberAccessNode>(identifier, member);
}


FunctionCallNodePtr JCAAnalyser::handle_function_call(nlohmann::json& function_call_json) {
	auto expression = function_call_json.at("expression");

	ASTNodePtr callee = nullptr;

	if (expression.contains("nodeType")) {
		std::string token = expression.at("nodeType");
		callee = get_value_equivalent_node(token, expression);
	}
	
	FunctionCallNodePtr function_call = std::make_shared<FunctionCallNode>(callee);

	// need update arguments !!!!!!
	auto arguments = function_call_json.at("arguments");
	for (size_t i = 0; i < arguments.size(); i++) {
		auto argument = arguments.at(i);
		if (argument.contains("nodeType")) {
			std::string token = argument.at("nodeType");
			function_call->add_argument(get_value_equivalent_node(token, argument));
		}
	}

	return function_call;
}


LiteralNodePtr JCAAnalyser::handle_literal(nlohmann::json& literal_json) {
	std::string literal_source = literal_json.at("value");
	LiteralNodePtr literal = std::make_shared<LiteralNode>(literal_source);
	return literal;
}


IdentifierNodePtr JCAAnalyser::handle_identifier(nlohmann::json& identifier_json) {
	std::string name = identifier_json.at("name");
	return std::make_shared<IdentifierNode>(name);
}


BinaryOperationNodePtr JCAAnalyser::handle_binary_operation(nlohmann::json& binary_op_json) {
	auto left_exp = binary_op_json.at("leftExpression");
	auto right_exp = binary_op_json.at("rightExpression");
	auto type_description = binary_op_json.at("typeDescriptions");

	std::string op = binary_op_json.at("operator");

	std::string type_str = type_description.at("typeString");
	ASTNodePtr left_hand, right_hand;

	std::string token;
	if (left_exp.contains("nodeType")) {
		token = left_exp.at("nodeType");
		left_hand = get_value_equivalent_node(token, left_exp);
	}
	
	if (right_exp.contains("nodeType")) {
		token = right_exp.at("nodeType");
		right_hand = get_value_equivalent_node(token, right_exp);
	}

	BinaryOperationNodePtr binary_operation = std::make_shared<BinaryOperationNode>(op, left_hand, right_hand);
	binary_operation->set_return_type_str(type_str);
	return binary_operation;
}


AssignmentNodePtr JCAAnalyser::handle_assignment(nlohmann::json& assignment_json) {
	std::string token;
	std::string op = assignment_json.at("operator");

	auto left_json = assignment_json.at("leftHandSide");
	auto right_json = assignment_json.at("rightHandSide");

	ASTNodePtr left = nullptr;
	ASTNodePtr right = nullptr;

	if (left_json.contains("nodeType")) {
		token = left_json.at("nodeType");
		left = get_value_equivalent_node(token, left_json);
	}
	
	if (right_json.contains("nodeType")) {
		token = right_json.at("nodeType");
		right = get_value_equivalent_node(token, right_json);
	}
	

	AssignmentNodePtr assignment = std::make_shared<AssignmentNode>(op);
	assignment->set_left_hand_operand(left);
	assignment->set_right_hand_operand(right);
	return assignment;
}


ParameterListNodePtr JCAAnalyser::handle_parameter_list(nlohmann::json& param_list_json) {
	ParameterListNodePtr parameters = std::make_shared<ParameterListNode>();

	auto params = param_list_json.at("parameters");
	for (size_t i = 0; i < params.size(); i++) {
		auto param = params.at(i);
		if (param.contains("nodeType")) {
			std::string token = param.at("nodeType");
			if (token == TokenVariableDeclaration) {
				VariableDeclarationNodePtr var_decl = handle_variable_declaration(param);
				parameters->add_parameter(var_decl);
			}
		}	
	}

	return parameters;
}


EnumDefinitionNodePtr JCAAnalyser::handle_enum_definition(nlohmann::json& enum_json) {
	EnumDefinitionNodePtr enum_definition = std::make_shared<EnumDefinitionNode>(enum_json.at("name"));

	auto members = enum_json.at("members");
	for (size_t i = 0; i < members.size(); i++) {
		auto member = members.at(i);
		
		if (member.contains("nodeType")) {
			if (member.at("nodeType") == TokenEnumValue) {
				EnumValueNodePtr enum_member = handle_enum_value(member);
				enum_definition->add_member(enum_member);
			}
		}
	}
	return enum_definition;
}

EnumValueNodePtr JCAAnalyser::handle_enum_value(nlohmann::json& enum_value_json) {
	std::string name = enum_value_json.at("name");
	return std::make_shared<EnumValueNode>(name);
}


VariableDeclarationNodePtr JCAAnalyser::handle_variable_declaration(nlohmann::json& var_json) {
	std::string variable_name = var_json.at("name");
	ASTNodePtr type;
	std::string type_str;

	type = get_type_name(var_json.at("typeName"));
	return std::make_shared<VariableDeclarationNode>(type, variable_name, type_str);
}

ASTNodePtr JCAAnalyser::get_type_name(nlohmann::json& type_name_json) {
	ASTNodePtr type_name;
	std::string type_str;

	if (type_name_json.contains("nodeType")) {
		std::string token = type_name_json.at("nodeType");

		if (token == TokenElementaryTypeName) {
			type_str = type_name_json.at("name");
			type_name = std::make_shared<ElementaryTypeNameNode>(type_str);
		}
		else if (token == TokenFunctionTypeName) {

		}
		else if (token == TokenArrayTypeName) {
			ASTNodePtr base_type_node = nullptr;
			ASTNodePtr index = nullptr;

			auto base_type = type_name_json.at("baseType");
			
			if(base_type.contains("nodeType"))
				base_type_node = get_type_name(base_type);

			//Need try to find out the index of the array

			type_name = std::make_shared<ArrayTypeNameNode>(base_type_node, index);
		}
		else if (token == TokenUserDefinedTypeName) {
			type_str = type_name_json.at("name");
			type_name = std::make_shared<UserDefinedTypeNameNode>(type_str);
		}
		else if (token == TokenMapping) {

		}
	}

	return type_name;
}

}