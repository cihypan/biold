<?
/*
   Author:    Jamie Telin (jamie.telin@gmail.com), currently at employed Zebramedia.se
    
   Scriptname: GoogleTranslateApi v1.1

   Use:        
           //Create new object
            $translate = new GoogleTranslateApi;
                
            //How it works
                $translate->FromLang = 'sv';
                $translate->ToLang = 'en';
                echo $translate->translate('Hej jag heter Jamie!');
                //Would output; Hello my name is Jamie!
                
                $translate->TranslatedText //Any translation is also saved in TranslatedText
            
            //Settings
                $translate->FromLang //Set language to translate from
                $translate->ToLang //Set language to translate to
                $translate->Text //Text to translate if not passed with translate();
            
            //Debug / Error reporting
                $translate->DebugMsg //Gets all error messages
                $translate->DebugStatus //Gets all status codes, 200 = ok, 400 = Invalid languages
                
    Important:
           //Google may update their API and change version. If so, you must update version number in this class.
            $translate->Version = '1.0'; //Use object to change version
            or
            var $Version = '1.0'; //Change it in the source of class
                
*/

class GoogleTranslateApi{

    var $BaseUrl = 'http://ajax.googleapis.com/ajax/services/language/translate';
    var $FromLang = 'sv';
    var $ToLang = 'en';
    var $Version = '1.0';
    
    var $CallUrl;
    
    var $Text = 'Hej världen!';
    
    var $TranslatedText;
    var $DebugMsg;
    var $DebugStatus;
    
    function GoogleTranslateApi(){
        $this->CallUrl = $this->BaseUrl . "?v=" . $this->Version . "&q=" . urlencode($this->Text) . "&langpair=" . $this->FromLang . "%7C" . $this->ToLang;
    }
    
    function makeCallUrl(){
        $this->CallUrl = $this->BaseUrl . "?v=" . $this->Version . "&q=" . urlencode($this->Text) . "&langpair=" . $this->FromLang . "%7C" . $this->ToLang;
    }
    
    function translate($text = ''){
        if($text != ''){
            $this->Text = $text;
        }
        $this->makeCallUrl();
        if($this->Text != '' && $this->CallUrl != ''){
            $handle = fopen($this->CallUrl, "rb");
            $contents = '';
            while (!feof($handle)) {
            $contents .= fread($handle, 8192);
            }
            fclose($handle);
            
            $json = json_decode($contents, true);
            
            if($json['responseStatus'] == 200){ //If request was ok
                $this->TranslatedText = $json['responseData']['translatedText'];
                $this->DebugMsg = $json['responseDetails'];
                $this->DebugStatus = $json['responseStatus'];
                return $this->TranslatedText;
            } else { //Return some errors
                return false;
                $this->DebugMsg = $json['responseDetails'];
                $this->DebugStatus = $json['responseStatus'];
            }
        } else {
            return false;
        }
    }
}
?> 
