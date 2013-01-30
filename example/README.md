Example components and testing data
==========================

The example is created by using one of the public DeSR models (Spanish) and the example default sentence in the demo [Taln parsing service at Pisa] (http://paleo.di.unipi.it/es/parse) http://paleo.di.unipi.it/es/parse

- DESR_UIMA: Contains an example UIMA component where that model can be used

- files: Contains input and output test files both in CONLL format for testing in DeSR command line, and xmi format for using within a UIMA pipeline

The files include the POS and feature tags (as well as lemmas) that the model is expecting, and the xmi example uses Barcelona Media's TypeSystem, adapted from the JulieLab TypeSystem.

To test the UIMA component, create a pipeline with the example  XMICasconsummer CAS Consummer, and use the DeSR component as an analysis engine. You can use the XMIWriter Collection Reader to check the results.
