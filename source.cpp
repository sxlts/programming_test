#include <iostream>
#include <stdlib.h> 
#include <string.h>
#include <vector>

int const years = 15;
double Brand_Factor = 1.5;
int const outputValuesNumber = 5;

struct Data
{
    int Agent_Breed = 0; // 0 - C, 1 - NC
    int Age;
    int Social_Grade;
    int Payment_at_Purchase;
    int Auto_Renew;
    int Inertia_for_Switch;    
    float Policy_ID;
    float Attribute_Brand;
    float Attribute_Price;
    float Attribute_Promotions;
};

void ProcessData(Data & data, int modelOutput[years][outputValuesNumber]) {
    int originalBreed = data.Agent_Breed;
    for (int year = 0; year < years; ++year) {
        if (data.Auto_Renew)
            continue;

        int previousBreed = data.Agent_Breed;

        double randNumber = (double)rand() / (double)RAND_MAX * 3.0;
        double Affinity = (double)data.Payment_at_Purchase / data.Attribute_Price + 
            (randNumber * data.Attribute_Promotions * (double)data.Inertia_for_Switch);
        if (data.Agent_Breed == 0 && Affinity < ((double)data.Social_Grade * data.Attribute_Brand))
            data.Agent_Breed = 1;
        else if (data.Agent_Breed == 1 && Affinity < ((double)data.Social_Grade * data.Attribute_Brand * Brand_Factor)) 
            data.Agent_Breed = 0;

        if (data.Agent_Breed == 0) // Breed_C Agents
            ++(modelOutput)[year][0];
        else                       // Breed_NC Agents
            ++(modelOutput)[year][1];

        if (previousBreed == 0 && data.Agent_Breed == 1) // Breed_C Lost (Switched to Breed_NC)
            ++(modelOutput)[year][2];
        if (previousBreed == 1 && data.Agent_Breed == 0) // Breed_C Gained (Switch from Breed_NC)
            ++(modelOutput)[year][3];

        if (originalBreed == 0 && previousBreed == 1 && data.Agent_Breed == 0) // Breed_C Regained (Switched to NC, then back to C)
            ++(modelOutput)[year][4];
    }
}

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    
    if (argc < 2) {
        std::cout << "give Brand_Factor and years value!\n";
        return 1;
    }

    Brand_Factor = atof(argv[1]);

    FILE * fin = fopen("PseudoData.csv", "r");
    if (!fin) {
        std::cout << "Error reading PseudoData.csv file";
        return 1;
    }

    fscanf(fin, "%*s"); // skip first line of .csv

    int modelOutput[years][outputValuesNumber] = {0};

    Data data;
    char Agent_Breed[20];
    while (fscanf(fin, "%[^,], %f, %d, %d, %d, %f, %f, %f, %d, %d", 
        Agent_Breed, &data.Policy_ID, &data.Age, &data.Social_Grade, 
        &data.Payment_at_Purchase, &data.Attribute_Brand, &data.Attribute_Price, 
        &data.Attribute_Promotions, &data.Auto_Renew, &data.Inertia_for_Switch) == 10)
    {
        if (strcmp(Agent_Breed, "Breed_NC") == 0)
            data.Agent_Breed = 1; // we assume that there are only two options and Breed_C is default
        ProcessData(data, modelOutput);
    }
    fclose(fin);


    FILE * fout = fopen("ModelOutput.csv", "w+");
    fprintf(fout, "%s,%s,%s,%s,%s\n", "Breed_C Agents", "Breed_NC Agents", "Breed_C Lost", "Breed_C Gained", "Breed_C Regained");
    for (int i = 0 ; i < years ; ++i)
        fprintf(fout, "%d,%d,%d,%d,%d\n", modelOutput[i][0], modelOutput[i][1], modelOutput[i][2], modelOutput[i][3], modelOutput[i][4]);
    fclose(fout);

    std::cout <<"Done!\n";
    
    return 0;
}