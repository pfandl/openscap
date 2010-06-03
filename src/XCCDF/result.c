/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */


#include "item.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "common/_error.h"
#include <math.h>
#include <text.h>


// constants
static const xccdf_numeric XCCDF_SCORE_MAX_DAFAULT = 100.0f;
static const char *XCCDF_INSTANCE_DEFAULT_CONTEXT = "undefined";
const size_t XCCDF_NUMERIC_SIZE = 32;
const char *XCCDF_NUMERIC_FORMAT = "%f";

// prototypes
void xccdf_rule_result_free(struct xccdf_rule_result *rr);
void xccdf_identity_free(struct xccdf_identity *identity);
void xccdf_score_free(struct xccdf_score *score);
void xccdf_target_fact_free(struct xccdf_target_fact *fact);
void xccdf_message_free(struct xccdf_message *msg);
void xccdf_instance_free(struct xccdf_instance *inst);
void xccdf_override_free(struct xccdf_override *oride);

struct xccdf_result *xccdf_result_new(void)
{
	struct xccdf_item *result = xccdf_item_new(XCCDF_RESULT, NULL);
	oscap_create_lists(&result->sub.result.identities, &result->sub.result.targets,
		&result->sub.result.remarks, &result->sub.result.target_addresses,
		&result->sub.result.target_facts, &result->sub.result.setvalues, &result->sub.result.organizations,
		&result->sub.result.rule_results, &result->sub.result.scores, NULL);
	return XRESULT(result);
}

struct xccdf_result * xccdf_result_clone(const struct xccdf_result * result)
{
	struct xccdf_item *new_result = oscap_calloc(1, sizeof(struct xccdf_item) + sizeof(struct xccdf_result_item));
	struct xccdf_item *old = XITEM(result);
	new_result->item = *(xccdf_item_base_clone(&(old->item)));
	new_result->type = old->type;
	new_result->sub.result = *(xccdf_result_item_clone(&(old->sub.result)));
	return XRESULT(new_result);
}

static inline void xccdf_result_free_impl(struct xccdf_item *result)
{
	if (result != NULL) {
		oscap_free(result->sub.result.test_system);
		oscap_free(result->sub.result.benchmark_uri);
		oscap_free(result->sub.result.profile);

		oscap_list_free(result->sub.result.identities, (oscap_destruct_func) xccdf_identity_free);
		oscap_list_free(result->sub.result.target_facts, (oscap_destruct_func) xccdf_target_fact_free);
		oscap_list_free(result->sub.result.scores, (oscap_destruct_func) xccdf_score_free);
		oscap_list_free(result->sub.result.targets, oscap_free);
		oscap_list_free(result->sub.result.remarks, (oscap_destruct_func) oscap_text_free);
		oscap_list_free(result->sub.result.target_addresses, oscap_free);
		oscap_list_free(result->sub.result.setvalues, (oscap_destruct_func) xccdf_setvalue_free);
		oscap_list_free(result->sub.result.rule_results, (oscap_destruct_func) xccdf_rule_result_free);
		oscap_list_free(result->sub.result.organizations, oscap_free);

		xccdf_item_release(result);
	}
}
XCCDF_FREE_GEN(result)

XCCDF_ACCESSOR_STRING(result, test_system)
XCCDF_ACCESSOR_STRING(result, benchmark_uri)
XCCDF_ACCESSOR_STRING(result, profile)
XCCDF_LISTMANIP(result, identity, identities)
XCCDF_LISTMANIP_STRING(result, target, targets)
XCCDF_LISTMANIP_STRING(result, target_address, target_addresses)
XCCDF_LISTMANIP_STRING(result, organization, organizations)
XCCDF_LISTMANIP_TEXT(result, remark, remarks)
XCCDF_LISTMANIP(result, target_fact, target_facts)
XCCDF_LISTMANIP(result, setvalue, setvalues)
XCCDF_LISTMANIP(result, rule_result, rule_results)
XCCDF_LISTMANIP(result, score, scores)
OSCAP_ITERATOR_GEN(xccdf_result)


struct xccdf_rule_result *xccdf_rule_result_new(void)
{
	struct xccdf_rule_result *rr = oscap_calloc(1, sizeof(struct xccdf_rule_result));
	oscap_create_lists(&rr->overrides, &rr->idents, &rr->messages,
		&rr->instances, &rr->fixes, &rr->checks, NULL);
	return rr;
}

void xccdf_rule_result_free(struct xccdf_rule_result *rr)
{
	if (rr != NULL) {
		oscap_free(rr->idref);
		oscap_free(rr->version);

		oscap_list_free(rr->overrides, (oscap_destruct_func) xccdf_override_free);
		oscap_list_free(rr->idents, (oscap_destruct_func) xccdf_ident_free);
		oscap_list_free(rr->messages, (oscap_destruct_func) xccdf_message_free);
		oscap_list_free(rr->instances, (oscap_destruct_func) xccdf_instance_free);
		oscap_list_free(rr->fixes, (oscap_destruct_func) xccdf_fix_free);
		oscap_list_free(rr->checks, (oscap_destruct_func) xccdf_check_free);

		oscap_free(rr);
	}
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_rule_result, time)
OSCAP_ACCESSOR_SIMPLE(xccdf_role_t, xccdf_rule_result, role)
OSCAP_ACCESSOR_SIMPLE(float, xccdf_rule_result, weight)
OSCAP_ACCESSOR_SIMPLE(xccdf_level_t, xccdf_rule_result, severity)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_rule_result, result)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, version)
OSCAP_ACCESSOR_STRING(xccdf_rule_result, idref)
OSCAP_IGETINS(xccdf_ident, xccdf_rule_result, idents, ident)
OSCAP_IGETINS(xccdf_fix, xccdf_rule_result, fixes, fix)
OSCAP_IGETTER(xccdf_check, xccdf_rule_result, checks)
OSCAP_IGETINS_GEN(xccdf_override, xccdf_rule_result, overrides, override)
OSCAP_IGETINS_GEN(xccdf_message, xccdf_rule_result, messages, message)
OSCAP_IGETINS_GEN(xccdf_instance, xccdf_rule_result, instances, instance)
OSCAP_ITERATOR_GEN(xccdf_rule_result)


struct xccdf_identity *xccdf_identity_new(void)
{
	return oscap_calloc(1, sizeof(struct xccdf_identity));
}

void xccdf_identity_free(struct xccdf_identity *identity)
{
	if (identity != NULL) {
		oscap_free(identity->name);
		oscap_free(identity);
	}
}

OSCAP_ACCESSOR_EXP(bool, xccdf_identity, authenticated, sub.authenticated)
OSCAP_ACCESSOR_EXP(bool, xccdf_identity, privileged, sub.privileged)
OSCAP_ACCESSOR_STRING(xccdf_identity, name)
OSCAP_ITERATOR_GEN(xccdf_identity)

struct xccdf_score *xccdf_score_new(void)
{
	struct xccdf_score *score = oscap_calloc(1, sizeof(struct xccdf_score));
	score->score = NAN;
	score->maximum = XCCDF_SCORE_MAX_DAFAULT;
	return score;
}

void xccdf_score_free(struct xccdf_score *score)
{
	if (score != NULL) {
		oscap_free(score->system);
		oscap_free(score);
	}
}

OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_score, maximum)
OSCAP_ACCESSOR_SIMPLE(xccdf_numeric, xccdf_score, score)
OSCAP_ACCESSOR_STRING(xccdf_score, system)
OSCAP_ITERATOR_GEN(xccdf_score)

struct xccdf_override *xccdf_override_new(void)
{
	return oscap_calloc(1, sizeof(struct xccdf_override));
}

void xccdf_override_free(struct xccdf_override *oride)
{
	if (oride != NULL) {
		oscap_free(oride->authority);
		oscap_text_free(oride->remark);
		oscap_free(oride);
	}
}

OSCAP_ACCESSOR_SIMPLE(time_t, xccdf_override, time)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_override, new_result)
OSCAP_ACCESSOR_SIMPLE(xccdf_test_result_type_t, xccdf_override, old_result)
OSCAP_ACCESSOR_STRING(xccdf_override, authority)
OSCAP_ACCESSOR_TEXT(xccdf_override, remark)

struct xccdf_message *xccdf_message_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_message));
}

void xccdf_message_free(struct xccdf_message *msg)
{
    if (msg != NULL) {
        oscap_free(msg->content);
        oscap_free(msg);
    }
}

OSCAP_ACCESSOR_SIMPLE(xccdf_message_severity_t, xccdf_message, severity)
OSCAP_ACCESSOR_STRING(xccdf_message, content)

struct xccdf_target_fact* xccdf_target_fact_new(void)
{
    return oscap_calloc(1, sizeof(struct xccdf_target_fact));
}

void xccdf_target_fact_free(struct xccdf_target_fact *fact)
{
    if (fact != NULL) {
        oscap_free(fact->name);
        oscap_free(fact->value);
        oscap_free(fact);
    }
}

static inline bool xccdf_target_fact_set_value(struct xccdf_target_fact *fact, xccdf_value_type_t type, const char *str)
{
    assert(fact != NULL);
    oscap_free(fact->value);
    fact->value = oscap_strdup(str);
    fact->type  = type;
    return true;
}

bool xccdf_target_fact_set_string(struct xccdf_target_fact *fact, const char *str)
{
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_STRING, str);
}

bool xccdf_target_fact_set_number(struct xccdf_target_fact *fact, xccdf_numeric val)
{
    char buff[XCCDF_NUMERIC_SIZE];
    sprintf(buff, XCCDF_NUMERIC_FORMAT, val);
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_NUMBER, buff);
}

bool xccdf_target_fact_set_boolean(struct xccdf_target_fact *fact, bool val)
{
    return xccdf_target_fact_set_value(fact, XCCDF_TYPE_BOOLEAN, oscap_enum_to_string(OSCAP_BOOL_MAP, val));
}

OSCAP_GETTER(xccdf_value_type_t, xccdf_target_fact, type)
OSCAP_GETTER(const char *, xccdf_target_fact, value)
OSCAP_ACCESSOR_STRING(xccdf_target_fact, name)
OSCAP_ITERATOR_GEN(xccdf_target_fact)

struct xccdf_instance *xccdf_instance_new(void)
{
    struct xccdf_instance *inst = oscap_calloc(1, sizeof(struct xccdf_instance));
    inst->context = oscap_strdup(XCCDF_INSTANCE_DEFAULT_CONTEXT);
    return inst;
}

void xccdf_instance_free(struct xccdf_instance *inst)
{
    if (inst != NULL) {
        oscap_free(inst->context);
        oscap_free(inst->parent_context);
        oscap_free(inst->content);
        oscap_free(inst);
    }
}

OSCAP_ACCESSOR_STRING(xccdf_instance, context)
OSCAP_ACCESSOR_STRING(xccdf_instance, parent_context)
OSCAP_ACCESSOR_STRING(xccdf_instance, content)

/* ------ string maps ------ */

const struct oscap_string_map XCCDF_FACT_TYPE_MAP[] = {
	{ XCCDF_TYPE_BOOLEAN, "boolean" },
	{ XCCDF_TYPE_STRING,  "string"  },
	{ XCCDF_TYPE_NUMBER,  "number"  },
	{ XCCDF_TYPE_BOOLEAN, NULL      }
};

const struct oscap_string_map XCCDF_RESULT_MAP[] = {
	{ XCCDF_RESULT_PASS,           "pass"          },
	{ XCCDF_RESULT_FAIL,           "fail"          },
	{ XCCDF_RESULT_ERROR,          "error"         },
	{ XCCDF_RESULT_UNKNOWN,        "unknown"       },
	{ XCCDF_RESULT_NOT_APPLICABLE, "notapplicable" },
	{ XCCDF_RESULT_NOT_CHECKED,    "notchecked"    },
	{ XCCDF_RESULT_NOT_SELECTED,   "notselected"   },
	{ XCCDF_RESULT_INFORMATIONAL,  "informational" },
	{ XCCDF_RESULT_FIXED,          "fixed"         },
	{ 0, NULL }
};

/* --------- import -------- */

static struct xccdf_identity *xccdf_identity_new_parse(xmlTextReaderPtr reader);
static struct xccdf_target_fact *xccdf_target_fact_new_parse(xmlTextReaderPtr reader);
static struct xccdf_score *xccdf_score_new_parse(xmlTextReaderPtr reader);
static struct xccdf_rule_result *xccdf_rule_result_new_parse(xmlTextReaderPtr reader);
static struct xccdf_override *xccdf_override_new_parse(xmlTextReaderPtr reader);
static struct xccdf_message *xccdf_message_new_parse(xmlTextReaderPtr reader);
static struct xccdf_instance *xccdf_instance_new_parse(xmlTextReaderPtr reader);

struct xccdf_result *xccdf_result_new_parse(xmlTextReaderPtr reader)
{
	assert(reader != NULL);
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_TESTRESULT);

	struct xccdf_item *res = XITEM(xccdf_result_new());

	if (!xccdf_item_process_attributes(res, reader)) goto fail;

	if (xccdf_attribute_has(reader, XCCDFA_END_TIME))
		res->sub.result.start_time = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_END_TIME));
	else goto fail;
	res->sub.result.start_time = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_START_TIME));
	res->item.version = xccdf_attribute_copy(reader, XCCDFA_VERSION);
	res->sub.result.test_system = xccdf_attribute_copy(reader, XCCDFA_TEST_SYSTEM);

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_ORGANIZATION:
			oscap_list_add(res->sub.result.organizations, oscap_element_string_copy(reader));
			break;
		case XCCDFE_IDENTITY:
			oscap_list_add(res->sub.result.identities, xccdf_identity_new_parse(reader));
			break;
		case XCCDFE_RESULT_PROFILE:
			if (res->sub.result.profile == NULL)
				res->sub.result.profile = oscap_element_string_copy(reader);
			break;
		case XCCDFE_TARGET:
			oscap_list_add(res->sub.result.targets, oscap_element_string_copy(reader));
			break;
		case XCCDFE_TARGET_ADDRESS:
			oscap_list_add(res->sub.result.target_addresses, oscap_element_string_copy(reader));
			break;
		case XCCDFE_TARGET_FACTS:
			while (oscap_to_start_element(reader, depth + 1)) {
				if (xccdf_element_get(reader) == XCCDFE_FACT)
					oscap_list_add(res->sub.result.target_facts, xccdf_target_fact_new_parse(reader));
				xmlTextReaderRead(reader);
			}
			break;
		case XCCDFE_SET_VALUE:
			oscap_list_add(res->sub.result.setvalues, xccdf_setvalue_new_parse(reader));
			break;
		case XCCDFE_RULE_RESULT:
			oscap_list_add(res->sub.result.rule_results, xccdf_rule_result_new_parse(reader));
			break;
		case XCCDFE_SCORE:
			oscap_list_add(res->sub.result.scores, xccdf_score_new_parse(reader));
			break;
		case XCCDFE_RESULT_BENCHMARK:
			if (res->sub.result.benchmark_uri == NULL)
				res->sub.result.benchmark_uri = xccdf_attribute_copy(reader, XCCDFA_HREF);
			break;
		default: xccdf_item_process_element(res, reader);
		}
		xmlTextReaderRead(reader);
	}

	return XRESULT(res);

fail:
	xccdf_result_free(XRESULT(res));
	return NULL;
}

int xccdf_result_export(struct xccdf_result *result, struct oscap_export_target *target)
{
	__attribute__nonnull__(target);

	int retcode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xccdf_result_to_dom(result, NULL, doc, NULL);

	retcode = xmlSaveFormatFileEnc(oscap_export_target_get_name(target), doc, oscap_export_target_get_encoding(target), 1);
	if (retcode < 1)
		oscap_setxmlerr(xmlGetLastError());

	xmlFreeDoc(doc);

	return retcode;
}

void xccdf_result_to_dom(struct xccdf_result *result, xmlNode *result_node, xmlDoc *doc, xmlNode *parent)
{
        xmlNs *ns_xccdf = NULL;
	if (parent) {
	        ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	} else {
		if (!result_node) result_node = xccdf_item_to_dom(XITEM(result), doc, parent);
                ns_xccdf = xmlNewNs(result_node, XCCDF_BASE_NAMESPACE, NULL);
		xmlDocSetRootElement(doc, result_node);
	}

	/* Handle attributes */
	/* start-time and end-time do not appear to be implemented in OpenSCAP */

	/* Handle children */
	struct oscap_text_iterator *remarks = xccdf_result_get_remarks(result);
	while (oscap_text_iterator_has_more(remarks)) {
		struct oscap_text *remark = oscap_text_iterator_next(remarks);
		xccdf_remark_to_dom(remark, doc, result_node);
	}
	oscap_text_iterator_free(remarks);

	struct oscap_string_iterator *orgs = xccdf_result_get_organizations(result);
	while (oscap_string_iterator_has_more(orgs)) {
		const char *org = oscap_string_iterator_next(orgs);
		xmlNewChild(result_node, ns_xccdf, BAD_CAST "organization", BAD_CAST org);
	}
	oscap_string_iterator_free(orgs);

	struct xccdf_identity_iterator *identities = xccdf_result_get_identities(result);
	while (xccdf_identity_iterator_has_more(identities)) {
		struct xccdf_identity *identity = xccdf_identity_iterator_next(identities);
		xmlNode *identity_node = xmlNewChild(result_node, ns_xccdf, BAD_CAST "identity", BAD_CAST xccdf_identity_get_name(identity));
		
		if (xccdf_identity_get_authenticated(identity))
			xmlNewProp(identity_node, BAD_CAST "authenticated", BAD_CAST "True");
		else
			xmlNewProp(identity_node, BAD_CAST "authenticated", BAD_CAST "False");

		if (xccdf_identity_get_privileged(identity))
			xmlNewProp(identity_node, BAD_CAST "privileged", BAD_CAST "True");
		else
			xmlNewProp(identity_node, BAD_CAST "privileged", BAD_CAST "False");
	}
	xccdf_identity_iterator_free(identities);

	const char *profile = xccdf_result_get_profile(result);
	if (profile) {
		xmlNode *prof_node = xmlNewChild(result_node, ns_xccdf, BAD_CAST "profile", NULL);
		xmlNewProp(prof_node, BAD_CAST "idref", BAD_CAST profile);
	}

	struct xccdf_setvalue_iterator *setvalues = xccdf_result_get_setvalues(result);
	while (xccdf_setvalue_iterator_has_more(setvalues)) {
		struct xccdf_setvalue *setvalue = xccdf_setvalue_iterator_next(setvalues);
		xccdf_setvalue_to_dom(setvalue, doc, result_node);
	}
	xccdf_setvalue_iterator_free(setvalues);

	struct oscap_string_iterator *targets = xccdf_result_get_targets(result);
	while (oscap_string_iterator_has_more(targets)) {
		const char *target = oscap_string_iterator_next(targets);
		xmlNewChild(result_node, ns_xccdf, BAD_CAST "target", BAD_CAST target);
	}
	oscap_string_iterator_free(targets);

	struct oscap_string_iterator *target_addresses = xccdf_result_get_target_addresses(result);
	while (oscap_string_iterator_has_more(target_addresses)) {
		const char *target_address = oscap_string_iterator_next(target_addresses);
		xmlNewChild(result_node, ns_xccdf, BAD_CAST "target_address", BAD_CAST target_address);
	}
	oscap_string_iterator_free(target_addresses);

	struct xccdf_target_fact_iterator *target_facts = xccdf_result_get_target_facts(result);
        if (xccdf_target_fact_iterator_has_more(target_facts)) {
                xmlNode *target_node = xmlNewChild(result_node, ns_xccdf, BAD_CAST "target_facts", NULL);
                while (xccdf_target_fact_iterator_has_more(target_facts)) {
                        struct xccdf_target_fact *target_fact = xccdf_target_fact_iterator_next(target_facts);
                        xmlNode *fact_node = xmlNewChild(target_node, ns_xccdf, BAD_CAST "fact", BAD_CAST target_fact->value);

                        const char *name = xccdf_target_fact_get_name(target_fact);
                        if (name)
                                xmlNewProp(fact_node, BAD_CAST "name", BAD_CAST name);

                        xccdf_value_type_t value = xccdf_target_fact_get_type(target_fact);
                        xmlNewProp(fact_node, BAD_CAST "type", BAD_CAST XCCDF_FACT_TYPE_MAP[value - 1].string);
                }
                xccdf_target_fact_iterator_free(target_facts);
        }

	struct xccdf_rule_result_iterator *rule_results = xccdf_result_get_rule_results(result);
	while (xccdf_rule_result_iterator_has_more(rule_results)) {
		struct xccdf_rule_result *rule_result = xccdf_rule_result_iterator_next(rule_results);
		xccdf_rule_result_to_dom(rule_result, doc, result_node);
	}
	xccdf_rule_result_iterator_free(rule_results);

	struct xccdf_score_iterator *scores = xccdf_result_get_scores(result);
	while (xccdf_score_iterator_has_more(scores)) {
		struct xccdf_score *score = xccdf_score_iterator_next(scores);

                float value = xccdf_score_get_score(score);
                char value_str[10];
                sprintf(value_str, "%f", value);
                xmlNode *score_node = xmlNewChild(result_node, ns_xccdf, BAD_CAST "score", BAD_CAST value_str);

		const char *sys = xccdf_score_get_system(score);
		if (sys)
			xmlNewProp(score_node, BAD_CAST "system", BAD_CAST sys);

		float max = xccdf_score_get_maximum(score);
		char max_str[10];
		sprintf(max_str, "%f", max);
		xmlNewProp(score_node, BAD_CAST "maximum", BAD_CAST max_str);
	}
	xccdf_score_iterator_free(scores);
}

static struct xccdf_identity *xccdf_identity_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_IDENTITY);

	struct xccdf_identity *identity = xccdf_identity_new();
	identity->sub.authenticated = xccdf_attribute_get_bool(reader, XCCDFA_AUTHENTICATED);
	identity->sub.privileged    = xccdf_attribute_get_bool(reader, XCCDFA_PRIVILEDGED);
	identity->name              = oscap_element_string_copy(reader);
	return identity;
}

static struct xccdf_target_fact *xccdf_target_fact_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_FACT);

	struct xccdf_target_fact *fact = xccdf_target_fact_new();
	fact->type = oscap_string_to_enum(XCCDF_FACT_TYPE_MAP, xccdf_attribute_get(reader, XCCDFA_TYPE));
	fact->name = xccdf_attribute_copy(reader, XCCDFA_NAME);
	fact->value = oscap_element_string_copy(reader);
	return fact;
}

static struct xccdf_score *xccdf_score_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_SCORE);

	struct xccdf_score *score = xccdf_score_new();
	score->system = xccdf_attribute_copy(reader, XCCDFA_SYSTEM);
	if (xccdf_attribute_has(reader, XCCDFA_MAXIMUM))
		score->maximum = xccdf_attribute_get_float(reader, XCCDFA_MAXIMUM);
	else score->maximum = XCCDF_SCORE_MAX_DAFAULT;
	score->score = atof(oscap_element_string_get(reader));
	return score;
}

static struct xccdf_rule_result *xccdf_rule_result_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_RULE_RESULT);

	struct xccdf_rule_result *rr = xccdf_rule_result_new();

	rr->idref    = xccdf_attribute_copy(reader, XCCDFA_IDREF);
	rr->role     = oscap_string_to_enum(XCCDF_ROLE_MAP, xccdf_attribute_get(reader, XCCDFA_ROLE));
	rr->time     = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_TIME));
	rr->version  = xccdf_attribute_copy(reader, XCCDFA_VERSION);
	rr->weight   = xccdf_attribute_get_float(reader, XCCDFA_WEIGHT);
	rr->severity = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_SEVERITY));

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_RESULT:
			rr->result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_OVERRIDE:
			oscap_list_add(rr->overrides, xccdf_override_new_parse(reader));
			break;
		case XCCDFE_IDENT:
			oscap_list_add(rr->idents, xccdf_ident_parse(reader));
			break;
		case XCCDFE_MESSAGE:
			oscap_list_add(rr->messages, xccdf_message_new_parse(reader));
			break;
		case XCCDFE_INSTANCE:
			oscap_list_add(rr->instances, xccdf_instance_new_parse(reader));
			break;
		case XCCDFE_FIX:
			oscap_list_add(rr->fixes, xccdf_fix_parse(reader));
			break;
		case XCCDFE_CHECK:
			oscap_list_add(rr->checks, xccdf_check_parse(reader));
			break;
		default: break;
		}
		xmlTextReaderRead(reader);
	}

	return rr;
}

xmlNode *xccdf_rule_result_to_dom(struct xccdf_rule_result *result, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *result_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "rule-result", NULL);

	/* Handle attributes */
	const char *idref = xccdf_rule_result_get_idref(result);
	if (idref)
		xmlNewProp(result_node, BAD_CAST "idref", BAD_CAST idref);

	xccdf_role_t role = xccdf_rule_result_get_role(result);
	if (role != 0) 
            xmlNewProp(result_node, BAD_CAST "role", BAD_CAST XCCDF_ROLE_MAP[role - 1].string);

	time_t date = xccdf_rule_result_get_time(result);
	struct tm *lt = localtime(&date);
	char timestamp[] = "yyyy-mm-ddThh:mm:ss";
	snprintf(timestamp, sizeof(timestamp), "%4d-%02d-%02dT%02d:%02d:%02d",
		 1900 + lt->tm_year, 1 + lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	xmlNewProp(result_node, BAD_CAST "date", BAD_CAST timestamp);

	xccdf_level_t severity = xccdf_rule_result_get_severity(result);
	if (severity != 0)
            xmlNewProp(result_node, BAD_CAST "severity", BAD_CAST XCCDF_LEVEL_MAP[severity - 1].string);

	const char *version = xccdf_rule_result_get_version(result);
	if (version)
		xmlNewProp(result_node, BAD_CAST "version", BAD_CAST version);

	float weight = xccdf_rule_result_get_weight(result);
	char weight_str[10];
	sprintf(weight_str, "%f", weight);
	xmlNewProp(result_node, BAD_CAST "weight", BAD_CAST weight_str);

	/* Handle children */
	xccdf_test_result_type_t test_res = xccdf_rule_result_get_result(result);
	if (test_res != 0)
            xmlNewChild(result_node, ns_xccdf, BAD_CAST "result", BAD_CAST XCCDF_RESULT_MAP[test_res - 1].string);

	struct xccdf_override_iterator *overrides = xccdf_rule_result_get_overrides(result);
	while (xccdf_override_iterator_has_more(overrides)) {
		struct xccdf_override *override = xccdf_override_iterator_next(overrides);
		xccdf_override_to_dom(override, doc, result_node);
	}
	xccdf_override_iterator_free(overrides);

	struct xccdf_ident_iterator *idents = xccdf_rule_result_get_idents(result);
	while (xccdf_ident_iterator_has_more(idents)) {
		struct xccdf_ident *ident = xccdf_ident_iterator_next(idents);
		xccdf_ident_to_dom(ident, doc, result_node);
	}
	xccdf_ident_iterator_free(idents);

	struct xccdf_message_iterator *messages = xccdf_rule_result_get_messages(result);
	while (xccdf_message_iterator_has_more(messages)) {
		struct xccdf_message *message = xccdf_message_iterator_next(messages);
		const char *content = xccdf_message_get_content(message);
		xmlNode *message_node = xmlNewChild(result_node, ns_xccdf, BAD_CAST "message", BAD_CAST content);

                xccdf_level_t message_severity = xccdf_message_get_severity(message);
		if (message_severity != 0)
                    xmlNewProp(message_node, BAD_CAST "severity", BAD_CAST XCCDF_LEVEL_MAP[message_severity - 1].string);
	}
	xccdf_message_iterator_free(messages);

	struct xccdf_instance_iterator *instances = xccdf_rule_result_get_instances(result);
	while (xccdf_instance_iterator_has_more(instances)) {
		struct xccdf_instance *instance = xccdf_instance_iterator_next(instances);
		const char *content = xccdf_instance_get_content(instance);
		xmlNode *instance_node = xmlNewChild(result_node, ns_xccdf, BAD_CAST "instance", BAD_CAST content);
		
		const char *context = xccdf_instance_get_context(instance);
		if (context)
			xmlNewProp(instance_node, BAD_CAST "context", BAD_CAST context); 

		const char *parent_context = xccdf_instance_get_context(instance);
		if (parent_context)
			xmlNewProp(instance_node, BAD_CAST "parentContext", BAD_CAST context); 
	}
	xccdf_instance_iterator_free(instances);

	struct xccdf_fix_iterator *fixes = xccdf_rule_result_get_fixes(result);
	while (xccdf_fix_iterator_has_more(fixes)) {
		struct xccdf_fix *fix = xccdf_fix_iterator_next(fixes);
		xccdf_fix_to_dom(fix, doc, result_node);
	}
	xccdf_fix_iterator_free(fixes);

	struct xccdf_check_iterator *checks = xccdf_rule_result_get_checks(result);
	while (xccdf_check_iterator_has_more(checks)) {
		struct xccdf_check *check = xccdf_check_iterator_next(checks);
		xccdf_check_to_dom(check, doc, result_node);
	}
	xccdf_check_iterator_free(checks);

	return result_node;
}

static struct xccdf_override *xccdf_override_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_OVERRIDE);

	struct xccdf_override *override = xccdf_override_new();

	override->time      = oscap_get_datetime(xccdf_attribute_get(reader, XCCDFA_TIME));
	override->authority = xccdf_attribute_copy(reader, XCCDFA_AUTHORITY);

	int depth = oscap_element_depth(reader) + 1;

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_OLD_RESULT:
			override->old_result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_NEW_RESULT:
			override->new_result = oscap_string_to_enum(XCCDF_RESULT_MAP, oscap_element_string_get(reader));
			break;
		case XCCDFE_REMARK:
			if (override->remark == NULL)
				override->remark = oscap_text_new_parse(XCCDF_TEXT_PLAIN, reader);
			break;
		default: break;
		}
		xmlTextReaderRead(reader);
	}

	return override;
}

xmlNode *xccdf_override_to_dom(struct xccdf_override *override, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *override_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "override", NULL);

	/* Handle attributes */
	time_t date = xccdf_override_get_time(override);
	struct tm *lt = localtime(&date);
	char timestamp[] = "yyyy-mm-ddThh:mm:ss";
	snprintf(timestamp, sizeof(timestamp), "%4d-%02d-%02dT%02d:%02d:%02d",
		 1900 + lt->tm_year, 1 + lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	xmlNewProp(override_node, BAD_CAST "date", BAD_CAST timestamp);

	const char *authority = xccdf_override_get_authority(override);
	if (authority)
		xmlNewProp(override_node, BAD_CAST "authority", BAD_CAST authority);

	xccdf_test_result_type_t old = xccdf_override_get_old_result(override);
	xmlNewProp(override_node, BAD_CAST "old-result", BAD_CAST XCCDF_RESULT_MAP[old - 1].string);

	xccdf_test_result_type_t new = xccdf_override_get_new_result(override);
	xmlNewProp(override_node, BAD_CAST "new-result", BAD_CAST XCCDF_RESULT_MAP[new - 1].string);

	struct oscap_text *remark = xccdf_override_get_remark(override);
	if (remark)
		xccdf_remark_to_dom(remark, doc, override_node);

	return override_node;
}

xmlNode *xccdf_remark_to_dom(struct oscap_text *remark, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_xccdf = xmlSearchNsByHref(doc, parent, XCCDF_BASE_NAMESPACE);
	xmlNode *remark_node = xmlNewChild(parent, ns_xccdf, BAD_CAST "remark", BAD_CAST oscap_text_get_text(remark));

	const char *lang = oscap_text_get_lang(remark);
	if (lang)
		xmlNewProp(remark_node, BAD_CAST "xml:lang", BAD_CAST lang);

	return remark_node;
}

static struct xccdf_message *xccdf_message_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_MESSAGE);

	struct xccdf_message *msg = xccdf_message_new();
	msg->severity = oscap_string_to_enum(XCCDF_LEVEL_MAP, xccdf_attribute_get(reader, XCCDFA_SEVERITY));
	msg->content  = oscap_element_string_copy(reader);
	return msg;
}

static struct xccdf_instance *xccdf_instance_new_parse(xmlTextReaderPtr reader)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_INSTANCE);

	struct xccdf_instance *inst = xccdf_instance_new();
	if (xccdf_attribute_has(reader, XCCDFA_CONTEXT))
		xccdf_instance_set_context(inst, xccdf_attribute_get(reader, XCCDFA_CONTEXT));
	inst->parent_context = xccdf_attribute_copy(reader, XCCDFA_PARENTCONTEXT);
	inst->content        = oscap_element_string_copy(reader);
	return inst;
}

/* --------  DUMP ---------- */

static void xccdf_rule_result_dump(struct xccdf_rule_result *res, int depth)
{
	xccdf_print_depth(depth); printf("Rule result: %s\n", xccdf_rule_result_get_idref(res)); ++depth;
	xccdf_print_depth(depth); printf("version:   %s\n", xccdf_rule_result_get_version(res));
	xccdf_print_depth(depth); printf("weight:    %f\n", xccdf_rule_result_get_weight(res));
	/*
	xccdf_role_t role;
	time_t time;
	xccdf_level_t severity;
	xccdf_test_result_type_t result;
	struct oscap_list *overrides;
	struct oscap_list *idents;
	struct oscap_list *messages;
	struct oscap_list *instances;
	struct oscap_list *fixes;
	struct oscap_list *checks;
	*/
}

void xccdf_result_dump(struct xccdf_result *res, int depth)
{
	xccdf_print_depth(depth); printf("TestResult : %s\n", (res ? xccdf_result_get_id(res) : "(NULL)"));
	if (res != NULL) {
		++depth; xccdf_item_print(XITEM(res), depth);
		xccdf_print_depth(depth); printf("test system:   %s\n", xccdf_result_get_test_system(res));
		xccdf_print_depth(depth); printf("benchmark URI: %s\n", xccdf_result_get_benchmark_uri(res));
		xccdf_print_depth(depth); printf("profile ID:    %s\n", xccdf_result_get_profile(res));
		// start time, end time...
		//xccdf_print_depth(depth); printf("identities");
		//oscap_list_dump(XITEM(res)->result.identities, NULL, depth+1);
		xccdf_print_depth(depth); printf("targets");
		oscap_list_dump(XITEM(res)->sub.result.targets, xccdf_cstring_dump, depth+1);
		xccdf_print_depth(depth); printf("organizations");
		oscap_list_dump(XITEM(res)->sub.result.organizations, xccdf_cstring_dump, depth+1);
		xccdf_print_depth(depth); printf("remarks");
		xccdf_print_textlist(xccdf_result_get_remarks(res), depth+1, 80, "...");
		xccdf_print_depth(depth); printf("target addresses");
		oscap_list_dump(XITEM(res)->sub.result.target_addresses, xccdf_cstring_dump, depth+1);
		//xccdf_print_depth(depth); printf("target_facts");
		xccdf_print_depth(depth); printf("setvalues");
		oscap_list_dump(XITEM(res)->sub.result.setvalues, xccdf_setvalue_dump, depth+1);
		xccdf_print_depth(depth); printf("rule results");
		oscap_list_dump(XITEM(res)->sub.result.rule_results, (oscap_dump_func) xccdf_rule_result_dump, depth+1);
		//xccdf_print_depth(depth); printf("scores");
	}
}


