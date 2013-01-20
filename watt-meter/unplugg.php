<?php

	require "unpluggClass.php";

	define('PULSEkWH',75);
	// Your auth_token
	$token_API = "";
	// your meter_id
	$meter_ID = "";


	if(isset($_GET['pulsos'])){
		$pulsos = $_GET['pulsos'];

		$unplugg = new Unplugg($token_API, $meter_ID, PULSEkWH);
		$unplugg->sendPulses($pulsos);

	}
	else
		echo "Sem parametros";
?>