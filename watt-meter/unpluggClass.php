<?php

define('PULSEkWH',75);

class Unplugg {

	private $auth_token;
	private $meter_id;
	private $pulseKWh;
	
	public function __construct($auth = 0,$mid = 0, $kwh) {
        $this->auth_token = $auth;
        $this->meter_id = $mid;
		$this->pulseKWh = $kwh;
    }
	
	public function updateParams($auth = 0,$mid = 0) {
        $this->auth_token = $auth;
        $this->meter_id = $mid;
    }
	
	private function paramsJson($count = 0){
		$fields = array(
			'auth_token' => $this->auth_token,
			'meter_id' => $this->meter_id,
			'consumption_value' => pulse2watt($count),
		);
		
		return json_encode($fields);
	}
	
	private function pulse2watt($count){
		return ($count/$this->pulseKWh)*1000;
	}
	
	public function sendPulses($count = 0){
		if($this->auth_token > 0 && $this->meter_id > 0 && $count > 0){
			$json = $this->paramsJson($count);
			
			$ch = curl_init();
			curl_setopt($ch,CURLOPT_URL, $url.'http://unplu.gg/consumptions.json');
			curl_setopt($ch,CURLOPT_CUSTOMREQUEST, "POST");
			curl_setopt($ch,CURLOPT_RETURNTRANSFER,1);
			curl_setopt($ch,CURLOPT_HTTPHEADER, array( 
				'Host: unplu.gg',
				'Content-Type: application/json',                                                                                
				'Content-Length: ' . strlen($json))                                                                       
			);
			curl_setopt($ch,CURLOPT_POSTFIELDS, $json);    

			$result = curl_exec($ch);
			curl_close($ch);
			return $result;
		}
	}

}




?>