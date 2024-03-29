#include "defTipo.hpp"
#include "Simulador.hpp"
#include <cstdlib>
#include <iostream>
//#include <fstream>

using namespace std;


//Fun��es de Print
void printRobot(Simulador *simulador) {
		
	cout <<"POSICAO" << endl;
	cout << "posicao X: " << simulador->getPosX() << endl;
	cout << "posicao Y: " << simulador->getPosY() << endl;
	cout << "Angulo: "  << simulador->getAngle() << endl;
	cout<< endl;	
}

void printSensor(Simulador *simulador){
	
	double *sensores = simulador->readSensor(10,0);
	cout <<"SENSORES" << endl;
	for(int j = 0; j < 4; j ++){
		cout<< "Sensor " << j <<": "<< *(sensores + j) << endl;
	}
}
 
// modificacao R
int programaUsuario(double *sensores) {
	int acao;
	//sensores[0] direita; sensores[1] frontal; sensores[2]  esquerda; sensores[3] 			; 1 ativado; 0 desativado
	// acao 	0 -- vira 45�; 		1 -- vira -45� ; 		2 -- vira 90�	; 		3 -- anda p/ frente dist cm 
	if (sensores[1]<0.01)
		acao=3;
	else if (sensores[0]<0.01)
		acao=1;
	else if (sensores[2]<0.01)
		acao=0;
	else
		acao=2;				
	return acao;
}


/******************************************************************************\
*				  	Impressao na tela							 			 *
\******************************************************************************/
void impressao(populacao *pop , int gen) {
	cout <<"Geracao: "<< gen <<endl;
	cout <<"Individuo com melhor fitness: "<<pop->melhorIndividuo<<endl;
	cout <<"Fitness do melhor Individuo: "<<pop->maxFitness<<endl;
    cout <<"Media do Fitness da geracao: "<<pop->mediaFitness<<endl;
    cout <<"Taxa mutacao: "<< taxaMut <<endl;
    cout <<"Taxa crossover: "<< taxaCross <<endl<<endl<<endl;
}//impressao

/******************************************************************************\
*				  Menu					 			 *
\******************************************************************************/
int menu() {
	int op;
	do 
	{
		cout << "\t1 - Continuar de onde parou" << endl;
		cout << "\t0 - Reiniciar\n" << endl;
		cin >> op;
	}while (op != 0 && op != 1 );
	
	return op;
}

/******************************************************************************\
*				  	Inicializacao da populacao					 			 *
\******************************************************************************/
void inicializacao(int nroExec, int op) {
	
	switch(op)
	{
		case 0 : //Reiniciar
		{
			apaga_arquivos(nroExec);
			int gene, numIndiv = 0;
		
			while(numIndiv < tamPop) {
			 	for (gene = 0; gene < lcrom; gene++) {
		     		popVelha.indiv[numIndiv].cromossomo[gene] = randon->nextFloat(-1,1) ; 							
				}
		        popVelha.indiv[numIndiv].fitness = calcFitness( popVelha.indiv[numIndiv].cromossomo, 0);	// Armazena Fitness do Individuo
				numIndiv++;		
			}

			estatistica( &popVelha,0);
			break;
		}
		case 1 : //Continuar de onde parou
			cout << "Lendo a populacao salva ...\n" << endl;
			ler_pop(nroExec);
			ler_esn(nroExec);
			break;
	}
	
	impressao(&popVelha,0);
	
}

/******************************************************************************\
*				  	Inicializacao da populacao baseada ESN-SUP 	 			 *
\******************************************************************************/
void inicializacao_sup(int nroExec, int op) {
	
	switch(op)
	{
		case 0 : //Reiniciar
		{
			apaga_arquivos(nroExec);
			int gene, numIndiv = 0;	
			
				
			while(numIndiv < tamPop) {
			 	for (gene = 0; gene < lcrom; gene++) {
		     		popVelha.indiv[numIndiv].cromossomo[gene] = randon->nextFloat(-1,1); 							
				}
		        popVelha.indiv[numIndiv].fitness = calcFitness( popVelha.indiv[numIndiv].cromossomo, 0);	// Armazena Fitness do Individuo
				numIndiv++;		
			}
			//salv_pop(99);
			//ler_pop(99);
			ler_esn_sup(499);
			//salv_esn(88);
			//cout<<endl;
			//esn->printESN();		
		
			//system("PAUSE");
			
			estatistica( &popVelha,0);
			break;
		}
		case 1 : //Continuar de onde parou
			cout << "Lendo a populacao salva ...\n" << endl;
			ler_pop(nroExec);
			ler_esn(nroExec);
			break;
	}
	
	impressao(&popVelha,0);

}

/******************************************************************************\
*				  	Execu��o Algoritimo Genetico							 			 *
\******************************************************************************/
void algGen(int nroExec, int op) {
	int gen = 0;
	
	//inicializacao(nroExec, op);				// procedimento para inicializa��o das vari�veis e da popula��o 
	inicializacao_sup(nroExec, op);
	
	
	do {	
		gen = gen + 1; 				// n�mero de gera��es 

		geracao(gen);
		estatistica( &popNova , gen); 

		individuo *aux;
		aux = popVelha.indiv;
		popVelha = popNova;
		popNova.indiv = aux;
		
		//cout<<"Impressao do Resultados da Exec: "<< nroExec << endl;
		impressao(&popVelha,gen);
		//arq_saida(nroExec);
	} while ( gen < maxGen );
	calcTrajeto (popVelha.indiv[popVelha.melhorIndividuo].cromossomo, nroExec, gen);		//Calcula e salva a trajetoria do melhor indiv da ultima gera��o
	arq_saida(nroExec);				// salva dados
}


/******************************************************************************\
*				  	main							 			 			  *
\******************************************************************************/

int main(void) {
	
	int gen = 0;
	
	//Parametros do Treinamento da ESN
	//double** conjunto_stab;
	
	//int nrRodadas = 2;//500  // modificacao R -movido para defValor
	//int size_stab = 50; //50  // modificacao R -movido para defValor
	//int nrMov = 500;//500 //10  // modificacao R -movido para defValor
	
	//ESN - Treinamento	
	double** inputs = new double*[numMov * nrRodadas];
	double* outputs = new double[numMov * nrRodadas];
	
	//TREINAMENTO DA ESN	
	cout<<"***** Treinamento da ESN *****"<<endl;
	for(int rodadaAtual = 0; rodadaAtual < nrRodadas; rodadaAtual++) {
			
		//Simulador::Simulador(int tamX, int tamY, int raio, int posX, int posY, int ang, bool dynamicEnvironment, int maxGen)
		//Simulador *simulador = new Simulador(200, 200, 0, 20, 20, 90, dynamicEnvironment, maxGen); // modificacao R
		Simulador *simulador = new Simulador(120,200, dynamicEnvironment, maxGen, gen); // modificacao R
	
		//Declara��o da Rede Neural
		//ESNbp *ESN = new ESNbp(inputSize, repSize, outputSize, nrMov, con_density, spectral_radius_d, size_stab); //size_stab?
	
		
		//Vetores para movimentos, posi��es
		
		//double movimentos[nrMov];
		int *movimentos = new int [numMov]; //definir passo a passo
		int *posicoesX = new int [numMov]; 
		int *posicoesY = new int [numMov];
		//int *acoes = new int [nrMov];
		double *sensores = simulador->readSensor(10,0);
	
		//Declara��o da Rede Neural
		/*ESNbp * */
		esn = new ESNbp(inputSize, repSize, outputSize, numMov-size_stab-1, con_density, spectral_radius_d, size_stab);  // modificacao R
		//ESNbp(int n_inp_par, int n_hid_par, int n_out_par, int n_train_par, double con_density, double spectral_radius_d, int size_stab)
		//esn = new ESN(inputSize, repSize, outputSize, spectral_radius_d, con_density);
		int acao;
		double acao_v[4];
		
		for(int mov = 0; mov < numMov; mov++ ) { //nrMov
			//acao = acoes[mov];	 // modificacao R
			acao = programaUsuario(sensores);	// modificacao R
			// simulador->execute(acao, 10 , 0); // // modificacao R
			
			//Se o rob� bateu, gira
			if(!simulador->execute(acao, 10 ,0)){
				break;
			}
			//cout << "Movimento: " << mov << endl;
			//cout << "Acao: " << acao << endl;
			//printRobot(simulador);
			//sensores = simulador->readSensor(10, gen); //distancia medida sensor = 10	// modificacao R
			sensores = simulador->readSensor(10, 0); //distancia medida sensor = 10	// modificacao R
			//printSensor(simulador);
									
			//cout << (rodadaAtual+1) * (mov)  <<  endl;
        	//inputs[(rodadaAtual+1) * (mov+1)] = simulador->readSensor(10, gen); // modificacao R
        	inputs[(rodadaAtual+1) * (mov+1)] = sensores; // modificacao R
        	//cout<< inputs[(rodadaAtual+1) * (mov+1)][1] << endl;
			outputs[(rodadaAtual+1) * (mov+1)] = acao; 
			//cout<< outputs[(rodadaAtual+1) * (mov+1)] << endl; 
			//if( rodadaAtual <= size_stab) { //definir tamanho de estabiliza��o // modificacao R
			
			if( mov <= size_stab) { //definir tamanho de estabiliza��o // modificacao R
				esn->ESNstab(sensores); //conjunto de estabiliza��o, utilizado para "inicializar"
			}
			else{
			//} else if( rodadaAtual > size_stab){ //size_stab + tam_conj_treinamento // modificacao R
				// modificacao R
				for(int j = 0; j < outputSize; j++)
					acao_v[j]=0.0;
				acao_v[acao]=1.0;
				esn->addTrainSet(sensores, acao_v); // modificacao R
			} 		
		}
		
		//cout<< "* * FIM RODADA: " << rodadaAtual << " * *" << endl; 
		//cout<<"TREINAMENTO"<<endl;
		esn->ESNTrain();
		
					
		if(rodadaAtual == nrRodadas - 1){
			cout<<"Finalizou treinamento da ESN"<<endl;
			cout<<"Numero de Rodadas: "<< rodadaAtual<< endl;
			//esn->printESN();
			salv_esn_sup(rodadaAtual);
			salv_esn_sup(99);
			cout<<endl;		
		}	
	
	} //FIM TREINAMENTO ESN
	
	//ALGORITMO GEN�TICO
		
	int nroExec, num_ind;

	arq_media_fitness = new double [maxGen+1];
	arq_melhor_fitness = new double [maxGen+1];
	popVelha.indiv = new individuo [tamPop];
	popNova.indiv = new individuo [tamPop];
	for (num_ind = 0; num_ind < tamPop; num_ind++){
		popVelha.indiv[num_ind].cromossomo = new double [lcrom];
		popNova.indiv[num_ind].cromossomo = new double [lcrom];
	}
	arq_melhor_individuo = aloc_matrixd(maxGen+1,lcrom);	
	
	// Execucao	
	cout<<"***** Algoritmo Genetico *****"<<endl;
	//int op = menu();	
	int op = 0;
	
	for(nroExec = 0; nroExec < nroMaxExec; nroExec++) {	
		// Visualizacao
		cout<<"\tExecucao: "<<nroExec<<endl<<endl;
		randon = new Randon(1,nroExec+1);					// semente para gerar os numeros aleatorios
		srand(nroExec+1); 	// semente para gerar os numeros aleatorios								
		//esn = new ESNbp(inputSize, repSize, outputSize, spectral_radius_d, con_density);
		algGen(nroExec, op);
		
		//delete esn;
		delete randon;								// chama a execucao do AG para uma semente aleatoria
	}//for
	
	cout<<"Desalocando Memoria do AG..."<<endl;
	// Desalocacao de Memoria
	delete [] arq_media_fitness;
	delete [] arq_melhor_fitness;
	for (num_ind=0; num_ind<tamPop; num_ind++){	
		delete [] popVelha.indiv[num_ind].cromossomo;
		delete [] popNova.indiv[num_ind].cromossomo;
	}
	delete [] popVelha.indiv;
	delete [] popNova.indiv;
	desaloc_matrixd(arq_melhor_individuo,maxGen+1);

	// Visualizacao 
	cout<<endl;
	cout<<"\tFim do programa!"<<endl;

	return 0;
}//main

