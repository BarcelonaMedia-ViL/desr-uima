/*    
 *  Copyright 2012 Fundació Barcelona Media
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 
 
#include "conf/conf_int.h"
#include "conf/conf_float.h"
#include "src/EventStream.h"
#include "State.h"
#include "MorphExtractor.h"
#include "WordCounts.h"
#include "text/WordSet.h"
#include "text/WordIndex.h"
 
#include <iostream>
#include <memory>
#include <string>
#include "uima/api.hpp"

#include "Corpus.h"
#include "Parser.h"
#include "Token.h"
#include "Language.h"
#include "TokenAttributes.h" 

using namespace uima;
using namespace Tanl;

class DeSR : public Annotator{

private:

	//Config params:
	
	// DeSR model
	std::string desr_model;
	std::string lang_code;

	icu::UnicodeString sentence_type;
	icu::UnicodeString token_type;
	icu::UnicodeString lemma_type;
	icu::UnicodeString pos_type;
	icu::UnicodeString cpos_type;
	icu::UnicodeString feats_type;
	icu::UnicodeString desr_type;
	
	icu::UnicodeString lemma_feature;
	icu::UnicodeString token_feature;
	icu::UnicodeString pos_feature;
	icu::UnicodeString cpos_feature;
	icu::UnicodeString feats_feature;
	
	icu::UnicodeString head_feature;
	icu::UnicodeString label_feature;
	icu::UnicodeString index_feature;
	icu::UnicodeString head_index_feature;
	icu::UnicodeString componentId_feature;
	
	//DeSR attribute flags specifed by end user
	bool tokenFlag;
	bool lemmaFlag;
	bool cposFlag;
	bool posFlag;   
	bool featsFlag;
	
	/////////////////////////
	
	bool token_feature_is_defined;

	
	//input types
	Type sentenceT;
	Type lemmaT;
	Type tokenT;
	Type cposT;
	Type posT;
	Type featT;
	
	//input features
	Feature tokenValueF;
	Feature lemmaValueF;
	Feature cposValueF;
	Feature posValueF;
	Feature featValueF;
	
	//output type
	Type desrT;
	
	//output features
	Feature headF;
	Feature indexF;
	Feature head_indexF;
	Feature labelF;
	Feature componentIdF;
	
	//DeSR attributes
	Tanl::Language *lang;
	Corpus *corpus;
	AttributeIndex corpusIndex;
	Attributes *attributes;
	Parser::Parser *parser;
	

public:

  DeSR(void){
  
		std::cout << "DeSR: Constructor" << std::endl;
		lang = new Tanl::Language("V", "N", "P");
  }

  ~DeSR(void){

		getAnnotatorContext().getLogger().logMessage("DeSR Destructor");
		std::cout << "DeSR: Destructor" << std::endl;
  }

  /** */
  TyErrorId initialize(AnnotatorContext & rclAnnotatorContext){
  
		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("DeSR initialize");
	  
		std::cout << "DeSR: initialize()" << std::endl;
		
		std::stringstream converter;

		if(!rclAnnotatorContext.isParameterDefined("desr_model") || rclAnnotatorContext.extractValue("desr_model", desr_model) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"desr_model\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("desr_model = '" + desr_model + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("sentence_type") || rclAnnotatorContext.extractValue("sentence_type", sentence_type) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"sentence_type\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("sentence_type = '" + sentence_type + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("token_type") || rclAnnotatorContext.extractValue("token_type", token_type) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"token_type\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("token_type = '" + token_type + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("lemma_type") || rclAnnotatorContext.extractValue("lemma_type", lemma_type) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"lemma_type\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("lemma_type = '" + lemma_type + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("pos_type") || rclAnnotatorContext.extractValue("pos_type", pos_type) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"pos_type\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("pos_type = '" + pos_type + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("cpos_type") || rclAnnotatorContext.extractValue("cpos_type", cpos_type) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"cpos_type\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("cpos_type = '" + cpos_type + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("feats_type") || rclAnnotatorContext.extractValue("feats_type", feats_type) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"feats_type\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("feats_type = '" + feats_type + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("desr_type") || rclAnnotatorContext.extractValue("desr_type", desr_type) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"desr_type\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("desr_type = '" + desr_type + "'");

   
        if(!rclAnnotatorContext.isParameterDefined("token") || rclAnnotatorContext.extractValue("token", tokenFlag) != UIMA_ERR_NONE){
          /* log the error condition */
          rclAnnotatorContext.getLogger().logError("Required configuration parameter \"tokenFlag\" not found in component descriptor");
          std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
          return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
        }
	       
        converter.str("");
	    converter << tokenFlag;
	
        /* log the configuration parameter setting */
        rclAnnotatorContext.getLogger().logMessage("token = '" + converter.str() + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("lemma") || rclAnnotatorContext.extractValue("lemma", lemmaFlag) != UIMA_ERR_NONE){
			/* log the error condition */
			rclAnnotatorContext.getLogger().logError("Required configuration parameter \"lemmaFlag\" not found in component descriptor");
			std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;

			return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
        }
	
        converter.str("");
        converter << lemmaFlag;
	       
        /* log the configuration parameter setting */
        rclAnnotatorContext.getLogger().logMessage("lemma = '" + converter.str() + "'");
	       
        if(!rclAnnotatorContext.isParameterDefined("cpos") || rclAnnotatorContext.extractValue("cpos", cposFlag) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"cposFlag\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}
	
		converter.str("");
		converter << cposFlag;
	       
		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("cpos = '" + converter.str() + "'");
	   
		if(!rclAnnotatorContext.isParameterDefined("pos") || rclAnnotatorContext.extractValue("pos", posFlag) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"posFlag\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		converter.str("");
		converter << posFlag;
	       
		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("pos = '" + converter.str() + "'");
	   
		if(!rclAnnotatorContext.isParameterDefined("feats") || rclAnnotatorContext.extractValue("feats", featsFlag) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"featsFlag\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		converter.str("");
		converter << featsFlag;
	   
		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("feats = '" + converter.str() + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("lemma_feature") || rclAnnotatorContext.extractValue("lemma_feature", lemma_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"lemma_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("lemma_feature = '" + lemma_feature + "'");
		
		
		if(rclAnnotatorContext.isParameterDefined("token_feature")){
		
			if(rclAnnotatorContext.extractValue("token_feature", token_feature) != UIMA_ERR_NONE){
			
				  /* log the error condition */
				  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"token_feature\" not found in component descriptor");
				  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
				  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
			}
		
			/* log the configuration parameter setting */
			rclAnnotatorContext.getLogger().logMessage("token_feature = '" + token_feature + "'");
			
			token_feature_is_defined = true;
		}
		else{
		
			/* log the configuration parameter setting */
			rclAnnotatorContext.getLogger().logMessage("token_feature has not been defined");
			
			token_feature_is_defined = false;
		}
		
		
		if(!rclAnnotatorContext.isParameterDefined("pos_feature") || rclAnnotatorContext.extractValue("pos_feature", pos_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"pos_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("pos_feature = '" + pos_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("cpos_feature") || rclAnnotatorContext.extractValue("cpos_feature", cpos_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"cpos_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("cpos_feature = '" + cpos_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("feats_feature") || rclAnnotatorContext.extractValue("feats_feature", feats_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"feats_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("feats_feature = '" + feats_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("head_feature") || rclAnnotatorContext.extractValue("head_feature", head_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"head_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("head_feature = '" + head_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("label_feature") || rclAnnotatorContext.extractValue("label_feature", label_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"label_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("label_feature = '" + label_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("index_feature") || rclAnnotatorContext.extractValue("index_feature", index_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"index_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("index_feature = '" + index_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("head_index_feature") || rclAnnotatorContext.extractValue("head_index_feature", head_index_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"head_index_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("head_index_feature = '" + head_index_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("componentId_feature") || rclAnnotatorContext.extractValue("componentId_feature", componentId_feature) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"componentId_feature\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}	

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("componentId_feature = '" + componentId_feature + "'");
		
		
		if(!rclAnnotatorContext.isParameterDefined("LanguageCode") || rclAnnotatorContext.extractValue("LanguageCode", lang_code) != UIMA_ERR_NONE){
		  /* log the error condition */
		  rclAnnotatorContext.getLogger().logError("Required configuration parameter \"lang_code\" not found in component descriptor");
		  std::cout << "DeSR: initialize() - Error. See logfile." << std::endl;
		  return UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT;
		}

		/* log the configuration parameter setting */
		rclAnnotatorContext.getLogger().logMessage("LanguageCode = '" + lang_code + "'");
		
		
		//DeSR intialization
		corpus = Corpus::create(*lang, "CoNLL");
		
		//Use the index from the Corpus
		corpusIndex = corpus->index;
		
		parser = Parser::Parser::create(desr_model.c_str());
		///////////////// ///////////////// /////////////////

		std::cout << "DeSR: initialize() .. desr_model: " << desr_model << std::endl;
		
		std::cout << "DeSR: initialize() .. sentence_type: " << sentence_type << std::endl;
		std::cout << "DeSR: initialize() .. token_type: " << token_type << std::endl;
		std::cout << "DeSR: initialize() .. lemma_type: " << lemma_type << std::endl;
		std::cout << "DeSR: initialize() .. pos_type: " << pos_type << std::endl;
		std::cout << "DeSR: initialize() .. cpos_type: " << cpos_type << std::endl;
		std::cout << "DeSR: initialize() .. feats_type: " << feats_type << std::endl;
		std::cout << "DeSR: initialize() .. desr_type: " << desr_type << std::endl;
		
		if(tokenFlag){
		
			std::cout << "DeSR: initialize() .. using token value" << std::endl;
		}
		if(lemmaFlag){
		
			std::cout << "DeSR: initialize() .. using lemma annotation" << std::endl;
		}
		if(cposFlag){
		
			std::cout << "DeSR: initialize() .. using cpos annotation" << std::endl;
		}
		if(posFlag){
		
			std::cout << "DeSR: initialize() .. using pos annotation" << std::endl;
		}
		if(featsFlag){
		
			std::cout << "DeSR: initialize() .. using feats annotation" << std::endl;
		}
		
		std::cout << "DeSR: initialize() .. lemma_feature: " << lemma_feature << std::endl;
		std::cout << "DeSR: initialize() .. token_feature: " << token_feature << std::endl;
		std::cout << "DeSR: initialize() .. pos_feature: " << pos_feature << std::endl;
		std::cout << "DeSR: initialize() .. cpos_feature: " << cpos_feature << std::endl;
		std::cout << "DeSR: initialize() .. feats_feature: " << feats_feature << std::endl;
		
		std::cout << "DeSR: initialize() .. head_feature: " << head_feature << std::endl;
		std::cout << "DeSR: initialize() .. label_feature: " << label_feature << std::endl;
		std::cout << "DeSR: initialize() .. index_feature: " << index_feature << std::endl;
		std::cout << "DeSR: initialize() .. head_index_feature: " << head_index_feature << std::endl;
		std::cout << "DeSR: initialize() .. componentId_feature: " << componentId_feature << std::endl;
		
		std::cout << "DeSR: initialize() .. language: " << lang_code << std::endl;

		return (TyErrorId)UIMA_ERR_NONE;
  }

  
  /** */
  TyErrorId typeSystemInit(TypeSystem const & crTypeSystem){
  
		getAnnotatorContext().getLogger().logMessage("DeSR typeSystemInit");
		
		std::cout << "DeSR: typeSystemInit()" << std::endl;
		
		//input types
		sentenceT = crTypeSystem.getType(sentence_type);
		tokenT = crTypeSystem.getType(token_type);
		lemmaT = crTypeSystem.getType(lemma_type);
		posT = crTypeSystem.getType(pos_type);
		cposT = crTypeSystem.getType(cpos_type);	
		featT = crTypeSystem.getType(feats_type);
		
		//output types and features
		desrT = crTypeSystem.getType(desr_type);

		
		if(!sentenceT.isValid()){
			
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Sentence Type object is not valid.");

			std::cout << "DeSR::typeSystemInit() - Error: Sentence Type object is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(!tokenT.isValid()){
			
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Token Type object is not valid.");

			std::cout << "DeSR::typeSystemInit() - Error: Token Type object is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(lemmaFlag && !lemmaT.isValid()){
			
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Lemma Type object is not valid.");

			std::cout << "DeSR::typeSystemInit() - Error: Lemma Type object is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(posFlag && !posT.isValid()){
			
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: POS Type object is not valid.");

			std::cout << "DeSR::typeSystemInit() - Error: POS Type object is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(cposFlag && !cposT.isValid()){
			
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: CPOS Type object is not valid.");

			std::cout << "DeSR::typeSystemInit() - Error: CPOS Type object is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(featsFlag && !featT.isValid()){
			
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Feat Type object is not valid.");

			std::cout << "DeSR::typeSystemInit() - Error: Feat Type object is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}

		//features
		lemmaValueF = lemmaT.getFeatureByBaseName(lemma_feature);
		
		if(token_feature_is_defined && token_feature.length() > 0){
		
			tokenValueF = tokenT.getFeatureByBaseName(token_feature);
		}
		else{
		
			token_feature_is_defined = false;
		}
		
		posValueF = posT.getFeatureByBaseName(pos_feature);      
		cposValueF = cposT.getFeatureByBaseName(cpos_feature);
		featValueF = featT.getFeatureByBaseName(feats_feature);	
		
		headF = desrT.getFeatureByBaseName(head_feature);
		labelF = desrT.getFeatureByBaseName(label_feature);
		indexF = desrT.getFeatureByBaseName(index_feature);
		head_indexF = desrT.getFeatureByBaseName(head_index_feature);
		componentIdF = desrT.getFeatureByBaseName(componentId_feature);
		
		if(lemmaFlag && !lemmaValueF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Lemma value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: Lemma value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(token_feature_is_defined && !tokenValueF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Token value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: Token value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(posFlag && !posValueF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Pos value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: Pos value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(cposFlag && !cposValueF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: CPos value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: CPos value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(featsFlag && !featValueF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Feat value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: Feat value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(!headF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: HeadF value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: HeadF value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(!labelF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: LabelF value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: LabelF value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(!indexF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: IndexF value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: IndexF value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(!head_indexF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: Head_indexF value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: Head_indexF value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}
		
		if(!componentIdF.isValid()){
		
			getAnnotatorContext().getLogger().logError("DeSR::typeSystemInit() - Error: componentIdF value feature is not valid.");
			std::cout << "DeSR::typeSystemInit() - Error: componentIdF value feature is not valid." << std::endl;
			return (TyErrorId) UIMA_ERR_RESMGR_INVALID_RESOURCE;
		}

		return (TyErrorId)UIMA_ERR_NONE;
  }

  
  /** */
  TyErrorId destroy(){
  
		getAnnotatorContext().getLogger().logMessage("DeSR destroy");
		
		std::cout << "DeSR: destroy()" << std::endl;
		return (TyErrorId)UIMA_ERR_NONE;
  }

  
  /** */
  TyErrorId process(CAS & tcas, ResultSpecification const & crResultSpecification){
  
		//std::cout << "DeSR: process() begins" << std::endl;
		FSIndexRepository & indexes = tcas.getIndexRepository();

		ANIndex sentenceIndex = tcas.getAnnotationIndex(sentenceT);
		
		ANIterator sentenceIter = sentenceIndex.iterator();
		sentenceIter.moveToFirst();
		
		getAnnotatorContext().getLogger().logMessage("DeSR process called");
		
		while(sentenceIter.isValid()){
		
			//Set of this sentence tokens
			std::vector<AnnotationFS> sentence_tokens;
		
			//Attributes index
			int index = 1;
			
			AnnotationFS sentence = sentenceIter.get();
			AnnotationFS token;
			AnnotationFS lemma;
			AnnotationFS cpos;
			AnnotationFS pos;
			AnnotationFS feat;

			ANIterator tokenIter = sentence.subIterator(tokenT);
			
			tokenIter.moveToFirst();
			Sentence *input = new Sentence(lang);
		
			while(tokenIter.isValid()){
		 
				attributes = new Attributes(&corpusIndex);	
				
				std::string tokenAttr;
				std::string lemmaAttr;
				std::string cposAttr;
				std::string posAttr;
				std::string featAttr;
				
				token = tokenIter.get();
				
				sentence_tokens.push_back(token);
                
				if(tokenFlag){
					
					if(!token_feature_is_defined){
					
						tokenAttr = token.getCoveredText().asUTF8();
					}
					else{
						
                        try{
                        
                            tokenAttr = token.getStringValue(tokenValueF).asUTF8();
                        }
                        catch(Exception & e){
                        
                            tokenAttr = "_";		
                            getAnnotatorContext().getLogger().logMessage("DeSR :: tokenAttr was found NULL for covered text: " + tokenAttr);
                        }
					}
				}
				else{
				
					tokenAttr = "_";
				}
				
				if(lemmaFlag){
							
                    try{
                        
                        if(lemmaT != tokenT){
                        
                            lemma = token.getFirstCoveringAnnotation(lemmaT);
                        }
                        else{
                        
                            lemma = token;
                        }
                        
                        lemmaAttr = lemma.getStringValue(lemmaValueF).asUTF8();			
                    }
                    catch(Exception & e){
                    
                        lemmaAttr = "_";
                        getAnnotatorContext().getLogger().logMessage("DeSR :: lemmaAttr was found NULL for covered text: " + tokenAttr);
                    }
				}
				else{
				
					lemmaAttr = "_";
				}
				
				if(cposFlag){
					
                    try{

                        if(cposT != tokenT){
                        
                            cpos = token.getFirstCoveringAnnotation(cposT);
                        }
                        else{
                        
                            cpos = token;
                        }
                        
                        cposAttr = cpos.getStringValue(cposValueF).asUTF8();
                    }
                    catch(Exception & e){
                    
                        cposAttr = "_";
                        getAnnotatorContext().getLogger().logMessage("DeSR :: cposAttr was found NULL for covered text: " + tokenAttr);
                    }
				}
				else{
				
					cposAttr = "_";
				}
				
				if(posFlag){
                
                    try{

                        if(posT != tokenT){
                         
                            pos = token.getFirstCoveringAnnotation(posT);
                        }
                        else{
                        
                            pos = token;
                        }
                            
                        posAttr = pos.getStringValue(posValueF).asUTF8();
                    }
                    catch(Exception & e){
                    
                        posAttr = "_";
                        getAnnotatorContext().getLogger().logMessage("DeSR :: posAttr was found NULL for covered text: " + tokenAttr);
                    }
				}
				else{
				
					posAttr = "_";
				}
				
				if(featsFlag){
					
                    try{

                        if(posT != tokenT){
                        
                            feat = token.getFirstCoveringAnnotation(featT);
                        }
                        else{
                        
                            feat = token;
                        }  
                            
                        featAttr = feat.getStringValue(featValueF).asUTF8();
                    }
                    catch(Exception & e){
                    
                        featAttr = "_";
                        getAnnotatorContext().getLogger().logMessage("DeSR :: featAttr was found NULL for covered text: " + tokenAttr);
                    }
				}
				else{
				
					featAttr = "_";
				}
		

				std::ostringstream oss;
				oss << index;
					
				attributes->insert("ID", oss.str());
				attributes->insert("LEMMA", lemmaAttr);
				attributes->insert("FORM", tokenAttr);
				attributes->insert("CPOSTAG", cposAttr);
				attributes->insert("POSTAG", posAttr);
				attributes->insert("FEATS", featAttr);
				attributes->insert("HEAD", "_");
				attributes->insert("DEPREL", "_");
				
				TreeToken *tt = new TreeToken(index, tokenAttr, *attributes);
				input->push_back(tt);
				
				index++;
				
				tokenIter.moveToNext();
			}
            
        
			Sentence *output = parser->parse(input);

			tokenIter.moveToFirst();
            
			for(int i=0; i<output->size(); ++i){
		
				TreeToken *mt = (TreeToken*) ((*output)[i]);
				
                //As DeSR results keeps the same order of token list, it extracts token by token when looping on DeSR results, 
                //as DeSR annotation span matches with token's.
				token = tokenIter.get();
				
				AnnotationFS dsr = tcas.createAnnotation(desrT, token.getBeginPosition(), token.getEndPosition());
				
				dsr.setStringValue(componentIdF, desr_type);
				
				std::string head;
				std::stringstream out;
				out << mt->linkHead();
				head = out.str();
				
				char* label_c = new char[mt->linkLabel().size() + 1];
				strcpy(label_c, mt->linkLabel().c_str());
				
				int token_index = mt->linkHead();

				if(token_index > 0){	
				
                    //Head feature set to 0 specifies that the word does not depend on any other word into the sentence.
					dsr.setFSValue(headF, sentence_tokens.at(token_index-1));
				}
		
				dsr.setIntValue(indexF, mt->id);
				dsr.setIntValue(head_indexF, mt->linkHead());
				dsr.setStringValue(labelF, label_c);
							
				indexes.addFS(dsr);
				
				tokenIter.moveToNext();
			}
		  
			sentenceIter.moveToNext();
			sentence_tokens.clear();
		}
		
		getAnnotatorContext().getLogger().logMessage("DeSR process completed");

		//std::cout << "DeSR: process() completed" << std::endl;
		return (TyErrorId)UIMA_ERR_NONE;
  }
  
};

// This macro exports an entry point that is used to create the annotator.

MAKE_AE(DeSR);
