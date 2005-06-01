#include "teseo_path.h"



// Crea nuovi strokes ruotati di un certo angolo espresso in radianti, attenzione, modifica strokes
void neuronSismos_ruotastrokes(glong num_strokes, gdouble* strokes, gdouble** pstrokes_ruotato, gdouble angolo) {
	// FILE *fstrokes;
	
  gdouble *strokes_ruotato;
  gint xl, yl, wl, hl;
  gdouble offx, offy;
  glong i;
  //g_message(" neuronSismos_ruotastrokes..inizio");

  strokes_ruotato = (gdouble *) malloc(sizeof(gdouble) * (num_strokes + 1) * 2);
  strokes_ruotato[num_strokes*2] = (gdouble) CANARY;

  if(!strokes_ruotato) {
    g_message("Not enough free memory for strokes_ruotato!");
    exit(1);
  }
  //Modificato il 03/05/02
  //  calcolagrandezzalayer(num_strokes, strokes, &xl, &yl, &wl, &hl);
  //
  //  offx = xl + (wl / 2);
  //  offy = yl + (hl / 2);
  offx=strokes[0];
  offy=strokes[1];

  //modifico strokes
  for(i=0; i < num_strokes ; i++) {
  	strokes[i*2] -= offx;
  	strokes[i*2 +1] -= offy;
  }


  gdouble cosangolo = cos(angolo);
  gdouble sinangolo = sin(angolo);

  for(i=0; i < num_strokes ; i++) {
  	strokes_ruotato[i*2]    = (cosangolo * strokes[i*2]) - (sinangolo * strokes[i*2+1]);
  	strokes_ruotato[i*2 +1] = (sinangolo * strokes[i*2]) + (cosangolo * strokes[i*2+1]);
  }	

	for(i=0; i < num_strokes ; i++) {
  	strokes[i*2] += offx;
  	strokes[i*2 +1] += offy;

  	strokes_ruotato[i*2] += offx;
  	strokes_ruotato[i*2 +1] += offy;
  }
  	
	*pstrokes_ruotato = strokes_ruotato;

  if(strokes_ruotato[num_strokes*2] != (gdouble) CANARY)
  	g_message("Canary in strokes_ruotato is dead!");
 	//g_message(" neuronSismos_ruotastrokes..fine");
}


/*Funzioni di I/O*/

//traduce un path_array in vettore nel quale non � indicato il tipo ma sono registrati anche i control
gdouble * path_array_to_strokes( gdouble * path_array, glong n_details, glong * n_strokes){
 glong lstrokes, numstrokes;
 gdouble * strokes=NULL;
 register int i=0;


 //g_message("path_array_to_strokes ..inizio");
 numstrokes =  ( n_details * 2 )/ 3;
 lstrokes = numstrokes * 2;

 strokes = ( gdouble * ) malloc ( sizeof(gdouble) * (lstrokes+1) );
 strokes[lstrokes] = (gdouble) CANARY;

 for(i=0; i < numstrokes; i++){
 	strokes[2*i]   = path_array[ i*3 ];
 	strokes[2*i+1] = path_array[ i*3+1 ];
 }

 if(strokes[lstrokes] != (gdouble) CANARY){
   g_message("Canary in path_array_to strokes is dead!");
 	 free(strokes);
 	 strokes=NULL;
 	 *n_strokes=0;
 }
 else {
  *n_strokes=numstrokes;
 }
 //g_message("path_array_to_strokes ..FINE");
 return strokes;
}


gdouble * open_path_to_array(gint32 g_image, glong* n_strokes,  char * nome_path){
  glong i=0, num_strokes, lstrokes;
  gdouble * points_pairs=NULL;
  gdouble * pstrokes_ret=NULL;
  gdouble * strokes_ret=NULL;

  gint path_type, path_closed, num_path_point_details;

  //ottengo il path
	gimp_path_get_points (g_image, nome_path, &path_closed, &num_path_point_details, &points_pairs);
  //se il path � corretto ricavo lo strokes
  if (!path_closed){
     //copio il vettore


     lstrokes = num_path_point_details ;
     //oppure togli questa riga e decommenta tutto il resto
     strokes_ret=points_pairs;
//     pstrokes_ret = (gdouble *) malloc( sizeof(gdouble) * ( lstrokes + 1 ) );
//     pstrokes_ret[lstrokes] = (gdouble) CANARY;
//
//     if( !pstrokes_ret ) {
//       g_message("Non riesco ad allocare strokes!");
//     }
//     else {
//
//      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
//      	g_message("Hai ucciso il canarino in pstrokes_ret!");
//      }
//      //copio nel pstrokes_ret
//     	for (i=0; i<lstrokes; i++)    {
//     		pstrokes_ret[i]   = points_pairs[i];
//     	}
//
//      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
//      	g_message("Hai ucciso il canarino in pstrokes_ret!");
//        if (pstrokes_ret)
//        	free(pstrokes_ret);
//        lstrokes=0;
//  	  }
//  	  else
//  	  {
//  	   strokes_ret=pstrokes_ret;
//  	  }
//     }

  }
  else {
   lstrokes=0;
   strokes_ret=NULL;
  }

  *n_strokes=lstrokes;
  return strokes_ret;

}

/**/

/* Dato uno strokes scrive il path*/
void strokes_to_open_path(gint32 g_image, glong num_strokes, gdouble *strokes, char * nome_path){

  gdouble * vet_punti=NULL;
  glong lpunti=0, i=0, j=0, k=0, kk=0, num_vet_punti;

  /* Se il numero di punti � multiplo di 3, gimp chiude automaticamente il PATH*/
  /*??Ridondante*/
  //  if (num_strokes%3)
  //  	{
  //  		num_strokes-=1;
  //  		printf("Tolgo un punto, num_strokes= %d",num_strokes);
  //  	}
  //Attenzione -3 aggiunto  il 14/01/2002
  lpunti = num_strokes * 3 * 3 - 3;
  /*
  g_message("num_strokes vale %d",num_strokes);
  */
  vet_punti = (gdouble *) malloc( ( sizeof(gdouble) ) * (lpunti + 1));
  if ( vet_punti != NULL && num_strokes > 2 ){

    vet_punti[lpunti] = (gdouble) CANARY;

    k=0;//conta i punti
   	for(j=0; j<2; j++) {
   		vet_punti[k*3]=strokes[0];
   		vet_punti[k*3+1]=strokes[1];
   		vet_punti[k*3+2]= (j == 0) ? 1.0 : 2.0;
   		k++;
   	}

    for (i=1; i<num_strokes && k<num_strokes*3; i++) {
    	for(j=0; j<3  && k<num_strokes*3; j++) {
    		vet_punti[k*3]=strokes[i*2];
    		vet_punti[k*3+1]=strokes[i*2+1];
    		vet_punti[k*3+2]= ( (j % 3) == 1) ? 1.0 : 2.0;
    		k++;
    	}
    }
    num_vet_punti = k;

    /*Stampo tutto*/
    printf("Path: %s", nome_path);
    printf("\nNum vet_punti: %d\n", k);
    if( num_vet_punti >  2) { //???
     gimp_path_set_points( g_image, nome_path, 1, num_vet_punti*3, vet_punti ); //gimp bug ??: vuole la dimensione del vettore
     //scrivo sull'immagine, se ho gi� creato un layer va a finire l�
     //gimp_path_stroke_current (g_image);
     //g_message(gimp_path_get_current(g_image));
    }
    if(vet_punti[lpunti] != (gdouble) CANARY)
    	g_message("Canary in vet_punti is dead!");
  }
  else {
     if(vet_punti!=NULL)
      g_message("Not enough free memory for vet_punti: %ld values", lpunti);
  }
  if (vet_punti)
  	free(vet_punti);

// gimp_displays_flush();

}


/* Dato un path ricava lo strokes corrispondente*/
gdouble * open_path_to_strokes(gint32 g_image, glong* n_strokes,  char * nome_path){
  glong i=0, num_strokes, lstrokes;
  gdouble * points_pairs=NULL;
  gdouble * pstrokes_ret=NULL;

  gint path_type, path_closed, num_path_point_details;

  //ottengo il path
	gimp_path_get_points (g_image, nome_path, &path_closed, &num_path_point_details, &points_pairs);
  //se il path � corretto ricavo lo strokes
  if (!path_closed){
     //alloco il vettore di strokes
     //+3 perch� per il primo punto scrive sei details, per gli altri nove
     num_strokes=(num_path_point_details+3)/9;

     lstrokes = num_strokes * 2;
     pstrokes_ret = (gdouble *) malloc( sizeof(gdouble) * (lstrokes + 1) );
     pstrokes_ret[lstrokes] = (gdouble) CANARY;

     if(!pstrokes_ret) {
       g_message("Not enough free memory for strokes!");
     }
     else {
      //copio nello strokes
      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
      	g_message("Canary in pstrokes_ret is dead!");
      }

     	for (i=0; i<num_strokes; i++)    {
     		pstrokes_ret[2*i]   = points_pairs[i*9];
     		pstrokes_ret[2*i+1] = points_pairs[i*9+1];    	
     	}
      if( pstrokes_ret[lstrokes] != (gdouble) CANARY ) {
      	g_message("Canary in pstrokes_ret is dead!");
        if (pstrokes_ret)
        	free(pstrokes_ret);
  	  }
     }
  }
  else {
   num_strokes=0;
   pstrokes_ret=NULL;
  }

  *n_strokes=num_strokes;
  return pstrokes_ret;

}



/*Funzioni di manipolazione path*/

void allinea_path(gint32 g_image)
{
	gchar **p;
	gint num_paths, num_points, num_points_tot = 0, path_closed;

	gdouble * path_strokes = NULL;
	gdouble * old_path[2];
	double a,b;

	int	num_point[2];
	int i=0;
	int j=0;
	int t=1;
	int k=0;
	int n_points_tot_eff=0;
	int n_point_eff[2];


 	p = gimp_path_list (g_image, &num_paths);

  while( num_paths>1)
	  {
		 n_points_tot_eff=0;
		 n_point_eff[2];

			for ( i=0; i<2; i++)
  			{
		  		gimp_path_get_points (g_image, p[i], &path_closed, &num_points, &old_path[i]);

					n_points_tot_eff+=num_points;
					n_point_eff[i]= num_points;

					num_point[i]= (num_points/3 -2)/3 +1;
//g_message("path %d  >>>>>>>  punti %d",t,n_point_eff[t]);
    		}

  		num_points_tot = num_point[0]	+	num_point[1];

			path_strokes = (gdouble *) malloc(sizeof(gdouble) * (n_points_tot_eff+1) );

      if(!path_strokes)
				g_message(" Not enough free memory for strokes.");


		a=  (old_path[1][n_point_eff[1]-3] - old_path[0][0] );      //scostamento delle x
		b=  -(old_path[1][n_point_eff[1]-2] - old_path[0][1] );		  //scostamento delle y


 	  	for (j=0; j<(n_point_eff[0]);j=j+3)
	 	  	{
	 	  		old_path[0][j]		+=a;
	 	  		old_path[0][j+1]	-=b;
//	 	  		old_path[0][j+2]	+=0;
			  }

   	  	for (t=1,k=0,j=0;	t>=0;	t--)
   	  	{
           		if(t==0)
         			j=3*2;
         			// numero di punti da saltare, per 3 ( x,y,tipo)...
       		 	//serve per eliminare il primo punto del tracciato che coincide con l'ultimo di quello percedente
  			for (j; j<n_point_eff[t] && k<n_points_tot_eff; j++, k++)
		    	  	path_strokes[k] = old_path[t][j];
	        }

		for ( i=0; i<2; i++)
				gimp_path_delete( g_image, p[i]);

		gimp_path_set_points( g_image, "UNIONE", 1, n_point_eff[0]+n_point_eff[1]-6, path_strokes );

		if (path_strokes)
  			free(path_strokes);

  		gimp_displays_flush();

    		p = gimp_path_list (g_image, &num_paths);
  	}
}

void unisci_path(gint32 g_image)
{
	gchar **p;
	gint num_paths, num_points, num_points_tot = 0, path_closed, k=0;	

	gdouble * path_strokes = NULL;
	gdouble * old_path;
	int i=0;
	int j=0;

	p = gimp_path_list (g_image, & num_paths);

  for ( i=0; i<=num_paths-1; i++)
  	{
	  gimp_path_get_points (g_image, p[i], &path_closed, &num_points, &old_path);
	  num_points_tot += (num_points+3-9)/3;
    }

  path_strokes = (gdouble *) malloc(sizeof(gdouble) * ( num_points_tot*3*3 + 1));

  for ( i=num_paths-1; i>=0; i--)
	 	{
		  gimp_path_get_points ( g_image, p[i], &path_closed, &num_points, &old_path);
      for ( j=0; j<num_points+3-9 && k<num_points_tot*3; j++, k++)
	      path_strokes[k] = old_path[j];
    }

  for ( i=0; i<=num_paths-1; i++)
	gimp_path_delete( g_image, p[i]);

	gimp_path_set_points( g_image, "UNIONE", 1, (num_points_tot-1)*3, path_strokes );

  if (path_strokes)
  	free(path_strokes);
}

void muovi_tracciato(gint32 g_image, gint x, gint y, gdouble rotate) {
 gdouble * strokes=NULL;
 gdouble *strokes_ruotato=NULL;
 gdouble * path_array=NULL;
 glong n_strokes, n_details;
 gint num_paths;
 gchar pathname [80];
 register int i;
 gdouble angolo_rad,PI_GRECO = acos(-1);

 angolo_rad=rotate*PI_GRECO/180.0;

 gimp_path_list (g_image, &num_paths);

 if (num_paths>0){
   //prendo il nome del path corrente
   strcpy(pathname, gimp_path_get_current(g_image));
   //traduco il path in array dello stessso formato di gimp
   path_array = open_path_to_array(g_image, &n_details,  pathname);

   if ( x!=0 || y!=0 )
   	{
     //modifico le coordinate contenute nell'array
     //il tipo non va toccato
     for (i=0; i< n_details / 3 ; i++) {
      path_array[i*3]+=x;
      path_array[i*3+1]+=y;
      //path_array[i*3+2]+=0;
     }
    }

   //eventuale rotazione
   if ( rotate != 0.0 )
    {
     //se devo ruotare prendo gli strokes e li modifico, poi copio le modifiche nel path_array
     strokes = path_array_to_strokes( path_array, n_details, &n_strokes);

     neuronSismos_ruotastrokes( n_strokes, strokes, &strokes_ruotato, -angolo_rad);

     //in strokes_ruotato ho scritto le modifiche da apportare al path_array
     //g_message("modifico il path_array");
     for (i=0; i< n_details / 3 ; i++) {
       path_array[i*3]   = strokes_ruotato[i*2];
       path_array[i*3+1] = strokes_ruotato[i*2+1];
       //path_array[i*3+2]+=0;
     }

     if(strokes)
      free(strokes);//non serve pi�
     if(strokes_ruotato)
      free(strokes_ruotato);//non serve pi�

    }

   gimp_path_delete(g_image,pathname);

   gimp_path_set_points(g_image, pathname, 1, n_details, path_array);

 }
  if(path_array)
   free(path_array);
}

//Modifica al 11/01/2002 Prevede che dall'esterno lo strokes passat sia consecutivo al path chiamante
//Non si controlla all'interno che il path e lo strokes non siano sovrapposti
/* Ricavo il vecchio path, vi concateno lo strokes*/
void concatena_path_strokes(gint32 g_image, glong num_strokes, gdouble *strokes){

  gdouble * old_path;
  gdouble * path_strokes=NULL;

  gint32 path_type, path_closed, num_path_point_details;
  char path_name [80] ;

  glong num_points;
  glong num_points_details=0, i=0, j=0, k=0, kk=0, num_vet_punti;
  gdouble ris=0;
  /*Ricavo il vecchio path*/

  //printf("\nPrima \n");
  strcpy(path_name,gimp_path_get_current(g_image));
  path_type = gimp_path_get_points( g_image, path_name, &path_closed, &num_path_point_details,  &old_path );
	//g_message("Nome del path %s", path_name);
  //printf("\nnumero di point_details: %d  path_type %d, path_closed %d ",num_path_point_details, path_type, path_closed);

  if ( !path_closed && num_path_point_details != 1 && path_type == 1) {
     //va tolto l'ultimo punto (-9) : correggere vanno presi tutti i punti

     num_points = ( num_path_point_details + 3 ) / 9 + num_strokes; //diviso 3 ???
     num_points_details = num_points * 9 - 3; //chiuso....
     // printf("num_path_point_details  = %d , num_new_strokes = %d , num_points_details = %d , num_strokes = %d", num_path_point_details, num_points, num_points_details, num_strokes);


     /*Se multiplo di tre gimp chiuderebbe il path*/
     /*Attenzione pare ridondante*/
     //     if ( (num_points%3) == 0)	{
     //    		  num_points-=1;
     //    		  num_strokes-=1;
     //    		  num_points_details-=9;
     //    		  g_message("\nTolgo un punto, num_points= %d\n",num_points);
     //    	}

    	
    // printf("num_points_details=num_points * 3 * 3 -3=%d",num_points_details);
    path_strokes = (gdouble *) malloc(sizeof(gdouble) * (num_points_details + 1));

    if(!path_strokes) {
      g_message("Not enough free memory for path_strokes!");
    }
    else {
     // printf("Allocato path_strokes: %d elementi\n", num_points_details);
     path_strokes[num_points_details] = (gdouble) CANARY;

    /* Copio in path_strokes, il path e lo strokes*/

    // printf("\nCopio la prima parte del vettore\n");
    //qui dentro copio tutto , da fuori gli passo sempre lo strokes corretto
    for( i=0; i < num_path_point_details; i++){
     path_strokes[i]=old_path[i];
    }
    k=i/3;    //poi riparto a scrivere nel vettore da k*3
    // printf("\nk= %d; i= %d\n", k,i);


    /*Copio lo strokes*/
//    printf("\nCopio lo strokes\n"); //parto dall'inizio; � il chiamante a garantire che strokes non ripeta i punti del vecchio path
    for (i=0; i<num_strokes; i++) {
//    	for(j=0; j<3 && k<(num_points_details-1); j++) {
    	for(j=0; j<3 ; j++) {    	
    		path_strokes[k*3]=strokes[i*2];
    		path_strokes[k*3+1]=strokes[i*2+1];
    		// ris = ( (j % 3) == 1 )? 1.0 : 2.0 ;
    		if (j % 3 == 1)
    			ris = 1.0;
    		else
    			ris = 2.0;
    		path_strokes[k*3+2] = ris;  	  	
    		k++;
    	}
    }
//    printf("\nCopio lo strokes ...fine\n");
    num_vet_punti = k;
    if ( num_vet_punti*3 != num_points_details)
		g_message("num_vet_punti*3 - 3 = %d num_points_details = %d",num_vet_punti*3, num_points_details);
		//g_message("num_points_details= %d , num_vet_punti*3= %d",num_points_details, num_vet_punti*3);
    if( num_vet_punti >  2) { //???

     gimp_path_delete(g_image,path_name);
		 //gimp_path_set_points( g_image, path_name, 1, (num_vet_punti)*3, path_strokes ); //gimp bug ??: vuole la dimensione del vettore
     gimp_path_set_points( g_image, path_name, 1, num_points_details , path_strokes ); //gimp bug ??: vuole la dimensione del vettore
    }

    }

    if(path_strokes[num_points_details] != (gdouble) CANARY)
    	g_message("Canary in path_strokes is dead!");
  }

  if (path_strokes)
  	free(path_strokes);

}


// void concatena_path_path_array(gint32 g_image, glong path_n, gdouble *path){
// 
//   gdouble * old_path;
//   gdouble * path_strokes=NULL;
//   gint32 path_type, vect_dim_get,path_closed;
//   char path_name [80] ;
//   char vicino=0;
//   glong num_points;
//   glong num_points_details=0, tot_details=0, i=0, j=0, k=0, kk=0, num_vet_punti, vect_dim,path_details, array_details;
//   gdouble ris=0;
// 
//   /*Ricavo il vecchio path*/
//   strcpy(path_name,gimp_path_get_current(g_image));
//   path_type = gimp_path_get_points( g_image, path_name, &path_closed, &vect_dim_get,  &old_path );
//
//   if ( !path_closed && vect_dim_get != 1 && path_type == 1)
//   {
// 
//     array_details = path_n;
//     path_details = vect_dim_get/3;
// 
//     ( distanza(old_path[vect_dim_get-6], old_path[vect_dim_get-5], path[0], path[1]) < 5) ? vicino=1: vicino=0;
//
//     if(vicino)
//     {
//     /*Per punti sufficientemente vicini butto il control dell'ultimo punto e butto il primo anchor del path successivo*/
//      tot_details = path_details + array_details - 2;
// 
//      //la dimensione del vettore da allocare
//      vect_dim= tot_details * 3;
// 
//      //g_message("vicino: path vecchio %ld vettore %ld path nuovo%ld tot_details %ld",vect_dim_get, array_details, vect_dim, tot_details);
//      path_strokes = (gdouble *) malloc(sizeof(gdouble) * (vect_dim + 1));
// 
//      if(!path_strokes) {
//       g_message("Not enough free memory for path_strokes!");
//      }
//      else {
//       // printf("Allocato path_strokes: %d elementi\n", num_points_details);
//       path_strokes[vect_dim] = (gdouble) CANARY;
// 
//       /* Copio in path_strokes il path tranne l'ultimo control*/
//       k=0;
//       for( i=0; i < (path_details - 1)*3 ; i++){
//        path_strokes[i]=old_path[i];
//       }
// 
//       k=i;   //k conta gli elementi del vettore
// 
//       /*Copio lo strokes saltando il primo anchor*/
//       for (i=0; i<(array_details-1)*3; i++) {
//        path_strokes[k+i]=path[i+1*(3)];
//       }
//      }
//     }
// 
//     else
//     {
//      /*Per punti sufficientemente distanti aggiungo il control del path successivo*/
//      tot_details = path_details + array_details + 1 ;
// 
//      //la dimensione del vettore da allocare
//      vect_dim= tot_details * 3;
// 
//      //g_message("lontano: path vecchio %ld vettore %ld path nuovo%ld tot_details %ld",vect_dim_get, array_details, vect_dim, tot_details);
//      path_strokes = (gdouble *) malloc(sizeof(gdouble) * (vect_dim + 1));
// 
//      if(!path_strokes) {
//       g_message("Not enough free memory for path_strokes!");
//      }
//      else {
//       // printf("Allocato path_strokes: %d elementi\n", num_points_details);
//       path_strokes[vect_dim] = (gdouble) CANARY;
// 
//       /* Copio in path_strokes il path */
//       k=0;
//       for( i=0; i < (path_details)*3 ; i++){
//        path_strokes[i]=old_path[i];
//       }
// 
//       k=i;   //k conta gli elementi del vettore
// 
//       /*Aggiungo un control*/
//        path_strokes[k]=path[0];
//        path_strokes[k+1]=path[1];
//        path_strokes[k+2]=2;/*Tipo control*/
//        k+=3;
// 
//       /*Copio lo strokes*/
//       for (i=0; i<(array_details)*3; i++) {
//        path_strokes[k+i]=path[i];
//       }
//      }
//     }
// 
//     if( vect_dim >  2) { //???
// 
//      gimp_path_delete(g_image,path_name);
//      //gimp_path_set_points( g_image, path_name, 1, (num_vet_punti)*3, path_strokes ); //gimp bug ??: vuole la dimensione del vettore
//      gimp_path_set_points( g_image, path_name, 1, vect_dim , path_strokes ); //gimp bug ??: vuole la dimensione del vettore
//     }
// 
// 
//     if(path_strokes[vect_dim] != (gdouble) CANARY)
//     	g_message("Canary in path_strokes is dead!");
//   }
// 
//   if (path_strokes)
//   	free(path_strokes);
// 
// }