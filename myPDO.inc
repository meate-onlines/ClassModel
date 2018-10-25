<?php

//的
/**
 * class myAccessPdo
 *
 * Description for class myAccessPdo
 *
 * @author:
 */
class myPDO extends PDO {

    private $enableError = true;
    public $result; //last query result
    public $lastQuery;
    private $loger;
    private static $in_flag = array(':id0', ':id1', ':id2', ':id3', ':id4', ':id5', ':id6', ':id7', ':id8', ':id9', ':id10', ':id11', ':id12', ':id13', ':id14', ':id15', ':id16', ':id17', ':id18', ':id19', ':id20', ':id21', ':id22', ':id23', ':id24', ':id25', ':id26', ':id27', ':id28', ':id29', ':id30', ':id31', ':id32', ':id33', ':id34', ':id35', ':id36', ':id37', ':id38', ':id39', ':id40', ':id41', ':id42', ':id43', ':id44', ':id45', ':id46', ':id47', ':id48', ':id49', ':id50', ':id51', ':id52', ':id53', ':id54', ':id55', ':id56', ':id57', ':id58', ':id59', ':id60', ':id61', ':id62', ':id63', ':id64', ':id65', ':id66', ':id67', ':id68', ':id69', ':id70', ':id71', ':id72', ':id73', ':id74', ':id75', ':id76', ':id77', ':id78', ':id79', ':id80', ':id81', ':id82', ':id83', ':id84', ':id85', ':id86', ':id87', ':id88', ':id89', ':id90', ':id91', ':id92', ':id93', ':id94', ':id95', ':id96', ':id97', ':id98', ':id99');
    private static $in_flag1 = array(':1id0', ':1id1', ':1id2', ':1id3', ':1id4', ':1id5', ':1id6', ':1id7', ':1id8', ':1id9', ':1id10', ':1id11', ':1id12', ':1id13', ':1id14', ':1id15', ':1id16', ':1id17', ':1id18', ':1id19', ':1id20', ':1id21', ':1id22', ':1id23', ':1id24', ':1id25', ':1id26', ':1id27', ':1id28', ':1id29', ':1id30', ':1id31', ':1id32', ':1id33', ':1id34', ':1id35', ':1id36', ':1id37', ':1id38', ':1id39', ':1id40', ':1id41', ':1id42', ':1id43', ':1id44', ':1id45', ':1id46', ':1id47', ':1id48', ':1id49', ':1id50', ':1id51', ':1id52', ':1id53', ':1id54', ':1id55', ':1id56', ':1id57', ':1id58', ':1id59', ':1id60', ':1id61', ':1id62', ':1id63', ':1id64', ':1id65', ':1id66', ':1id67', ':1id68', ':1id69', ':1id70', ':1id71', ':1id72', ':1id73', ':1id74', ':1id75', ':1id76', ':1id77', ':1id78', ':1id79', ':1id80', ':1id81', ':1id82', ':1id83', ':1id84', ':1id85', ':1id86', ':1id87', ':1id88', ':1id89', ':1id90', ':1id91', ':1id92', ':1id93', ':1id94', ':1id95', ':1id96', ':1id97', ':1id98', ':1id99');

    public function __destruct() {
        if ($this->loger)
            $this->loger->logfile_close();
    }

    public function __construct($host, $dbname, $user, $pass, $dbType = 'mysql') {

        try {
            if ($dbType == 'oci') {
                $oracle_home_path = "/usr/lib/oracle/11.1/client64/lib";
                if (is_dir($oracle_home_path))
                    putenv("ORACLE_HOME=" . $oracle_home_path);
                parent::__construct("$dbType:dbname=//$host/$dbname;charset=ZHS16GBK", $user, $pass);
                $this->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);
            }elseif ($dbType == 'dblib') {
                parent::__construct("$dbType:host=$host;dbname=$dbname;charset=utf8", $user, $pass);
            } else {
                $charset = array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES 'utf8';");
                parent::__construct("$dbType:host=$host;dbname=$dbname", $user, $pass, $charset);
            }
            if ($this->enableError) {
                $this->loger = new pdoLoger();
                $this->loger->logfile_init();
                $this->displayError();
            }
        } catch (PDOException $e) {
            $loger = new pdoLoger();
            $loger->logfile_init();
            $loger->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $e);
            $loger->logfile_close();
        }
    }

    private function bindParams($sth, $bindArrParams) {
        if (count($bindArrParams) > 0) {
            $key = array_keys($bindArrParams);
            if (!is_numeric($key[0]) && (substr($key[0], 0, 1) == ':')) {
                foreach ($bindArrParams as $keyParams => $valueParams) {
                    $sth->bindValue($keyParams, $valueParams);
                }
                $this->result = $sth->execute();
            } else {
                $this->result = $sth->execute($bindArrParams);
            }
        } else {
            $this->result = $sth->execute();
        }
        return $sth;
    }

    public function simplePrepare($query, $bindArrParams = array()) {
        try {
            $this->lastQuery = $query;
            $sth = parent::prepare($query);
            $sth = $this->bindParams($sth, $bindArrParams);
            $sth->closeCursor();
            return resultData::set($this->result, $this->lastQuery, $bindArrParams);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(array(), $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    //执行变更，返回修改行数
    public function changePrepare($query, $bindArrParams = array()) {
        try {
            $this->lastQuery = $query;
            $sth = parent::prepare($query);
            $sth = $this->bindParams($sth, $bindArrParams);
            $rowCount = $sth->rowCount();
            $sth->closeCursor(); // sblocco stment
            return resultData::set($rowCount, $this->lastQuery, $bindArrParams);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(-1, $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    //  $myDb->prepare('SELECT * from programmattori WHERE nome = ?',array('giggighe'));
    //  $myDb->prepare('SELECT * from programmattori WHERE nome = :cccc',array(':cccc'=>'giggighe'));

    public function insertPrepare($query, $bindArrParams = array()) {
        try {

            $this->lastQuery = $query;
            $sth = parent::prepare($query);
            $sth = $this->bindParams($sth, $bindArrParams);
            $sth->closeCursor();
            return resultData::set($this->lastInsertId(), $this->lastQuery, $bindArrParams);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(-1, $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    public function allPrepare($query, $bindArrParams = array()) {
        try {
            $this->lastQuery = $query;
            $sth = parent::prepare($query);
            $sth = $this->bindParams($sth, $bindArrParams);
            $this->result = $sth->fetchAll(PDO::FETCH_ASSOC);
            $sth->closeCursor();
            return resultData::set($this->result, $this->lastQuery, $bindArrParams);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(array(), $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    public function rowPrepare($query, $bindArrParams = array()) {
        try {
            $this->lastQuery = $query;
            $sth = parent::prepare($query);
            $sth = $this->bindParams($sth, $bindArrParams);
            $this->result = $sth->fetch(PDO::FETCH_ASSOC);
            $sth->closeCursor();
            return resultData::set($this->result, $this->lastQuery, $bindArrParams);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(array(), $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    public function allPrepare_in($query, $params, $other = array()) {
        try {
            $params = array_merge($params);
            if (empty($params))
                return -1;
            $number = count($params);
            if ($number < count(self::$in_flag)) {
                $sql_query = sprintf($query, implode(',', array_slice(self::$in_flag, 0, $number)));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
                for ($i = 0; $i < $number; $i++) {
                    $sth->bindValue(self::$in_flag[$i], $params[$i]);
                }
            } else {
                $sql_query = sprintf($query, implode(',', $params));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
            }
            if (!empty($other)) {
                foreach ($other as $key => $value) {
                    $sth->bindValue($key, $value);
                }
                $params = array_merge($params, $other);
            }
            $sth->execute();
            $this->result = $sth->fetchAll(PDO::FETCH_ASSOC);
            $sth->closeCursor();
            return resultData::set($this->result, $this->lastQuery, $params);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(array(), $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    public function changePrepare_in($query, $params, $other = array()) {
        try {
            $params = array_merge($params);
            if (empty($params))
                return -1;
            $number = count($params);
            if ($number < count(self::$in_flag)) {
                $sql_query = sprintf($query, implode(',', array_slice(self::$in_flag, 0, $number)));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
                for ($i = 0; $i < $number; $i++) {
                    $sth->bindValue(self::$in_flag[$i], $params[$i]);
                }
            } else {
                $sql_query = sprintf($query, implode(',', $params));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
            }
            if (!empty($other)) {
                foreach ($other as $key => $value) {
                    $sth->bindValue($key, $value);
                }
                $params = array_merge($params, $other);
            }
            $sth->execute();
            $rowCount = $sth->rowCount();
            $sth->closeCursor(); // sblocco stment
            return resultData::set($rowCount, $this->lastQuery, $params);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(array(), $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    //multi in params
    public function allPrepare_2in($query, $params1, $params2, $other = array()) {
        try {
            $params1 = array_merge($params1);
            $params2 = array_merge($params2);
            if (empty($params1) or empty($params2))
                return -1;
            $number1 = count($params1);
            $number2 = count($params2);
            $flag = count(self::$in_flag);
            $flag1 = count(self::$in_flag1);
            if ($number1 < $flag and $number2 < $flag1) {
                $sql_query = sprintf($query, implode(',', array_slice(self::$in_flag, 0, $number1)), implode(',', array_slice(self::$in_flag1, 0, $number2)));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
                for ($i = 0; $i < $number1; $i++) {
                    $sth->bindValue(self::$in_flag[$i], $params1[$i]);
                }
                for ($i = 0; $i < $number2; $i++) {
                    $sth->bindValue(self::$in_flag1[$i], $params2[$i]);
                }
            } else {
                $sql_query = sprintf($query, implode(',', $params1), implode(',', $params2));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
            }
            if (!empty($other)) {
                foreach ($other as $key => $value) {
                    $sth->bindValue($key, $value);
                }
                $params = array_merge($params, $other);
            }
            $sth->execute();
            $this->result = $sth->fetchAll(PDO::FETCH_ASSOC);
            $sth->closeCursor();
            return resultData::set($this->result, $this->lastQuery, $params);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(array(), $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    public function changePrepare_2in($query, $params1, $params2, $other = array()) {
        try {
            $params1 = array_merge($params1);
            $params2 = array_merge($params2);
            if (empty($params1) or empty($params2))
                return -1;
            $number1 = count($params1);
            $number2 = count($params2);
            $flag = count(self::$in_flag);
            $flag1 = count(self::$in_flag1);
            if ($number1 < $flag and $number2 < $flag1) {
                $sql_query = sprintf($query, implode(',', array_slice(self::$in_flag, 0, $number1)), implode(',', array_slice(self::$in_flag1, 0, $number2)));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
                for ($i = 0; $i < $number1; $i++) {
                    $sth->bindValue(self::$in_flag[$i], $params1[$i]);
                }
                for ($i = 0; $i < $number2; $i++) {
                    $sth->bindValue(self::$in_flag1[$i], $params2[$i]);
                }
            } else {
                $sql_query = sprintf($query, implode(',', $params1), implode(',', $params2));
                $this->lastQuery = $sql_query;
                $sth = parent::prepare($sql_query);
            }
            if (!empty($other)) {
                foreach ($other as $key => $value) {
                    $sth->bindValue($key, $value);
                }
                $params = array_merge($params, $other);
            }
            $sth->execute();
            $rowCount = $sth->rowCount();
            $sth->closeCursor(); // sblocco stment
            return resultData::set($rowCount, $this->lastQuery, $params);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
            $result = new resultData(array(), $query, $bindArrParams);
            $result->set_error($e);
            return $result;
        }
    }

    public function simpleQuery($query) {
        try {
            $this->lastQuery = $query;
            $count = parent::exec($query);
            return $count;
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $this->lastQuery);
        }
    }

    public function lastInsertId($name = '') {
        try {
            $id = 0;
            if (parent::getAttribute(PDO::ATTR_DRIVER_NAME) == 'dblib') {
                $sth = parent::prepare("SELECT @@IDENTITY AS ID");
                $sth->execute();
                $result = $sth->fetch(PDO::FETCH_ASSOC);
                $sth->closeCursor();
                return $result[0];
            }


            $id = parent::lastInsertId($name);
            return $id;
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $e);
        }
    }

    public function closeConnection() {
        try {
            $this->dbh = null;
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $e);
        }
    }

    public function displayError() {
        try {
            parent::setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        } catch (PDOException $e) {
            $this->logErr(__FUNCTION__ . " Error!: " . $e->getMessage() . "\n", $e);
        }
    }

    public function getErrorCode() {
        return parent::errorCode();
    }

    public function getErrorInfo() {
        return parent::errorInfo();
    }

    public function logErr($str, $e) {
        $this->loger->logErr($str, $e);
    }

}

class pdoLoger {

    private $LOGFILE = "";
    var $FILE_HANDLER;

    static function forceDirectory($dir) { // force directory structure
        return is_dir($dir) or ( pdoLoger::forceDirectory(dirname($dir)) and mkdir($dir, 0777));
    }

    function logfile_init() {
        if (empty($this->LOGFILE)) {
            $filename = Conf::$log['log_path'] . 'pdo' . date('Ymd') . '.log';
            $this->LOGFILE = $filename;
        } else {
            $filename = $this->LOGFILE;
        }
        pdoLoger::forceDirectory(Conf::$log['log_path']);
        $this->FILE_HANDLER = fopen($filename, 'a');
    }

    public function logErr($str, $e) {
        $file_size = filesize($this->LOGFILE);
        if ($file_size > 1000000000)
            return;
        $err = "\n" . date(("Ymd H:i:s")) . "-->" . $str . ":::" . print_r($e, true) . "<--";
        substr($err, 0, 15000);
        if ($this->FILE_HANDLER) {
            fwrite($this->FILE_HANDLER, $err); //暂时用写入log，空间不够
        }
    }

    public function logfile_close() {
        if ($this->FILE_HANDLER) {
            fclose($this->FILE_HANDLER);
        }
    }

}

class resultData {

    private $query = null;
    private $par = null;
    private $data = array();
    private $error = null;

    public function __construct($data, $query, $par) {
        $this->data = $data;
        $this->query = $query;
        $this->par = $par;
    }

    function __get($data) {
        return $this->$data;
    }

    public function get() {
        return $this->data;
    }

    public function debug() {
        echo "<pre>";
        print_r($this);
        echo "</pre>";
    }

    public function getQuery() {
        return $this->query;
    }

    public function count() {
        if (is_int($this->data))
            return $this->data;
        return count($this->data);
    }

    public function isEmpty() {
        return empty($this->data);
    }

    public function row($index = 0) {
        return $this->data[$index];
    }

    public static function set($data, $query, $par) {
        return new resultData($data, $query, $par);
    }

    public function set_error($e) {
        $this->error = $e;
    }

    public function resultId() {
        if (!is_array($this->data))
            return $this->data;
    }

}

class nullPDO {

    public function rowPrepare($query, $bindArrParams = array()) {
        $result = new resultData(array(), $query, $bindArrParams);
        $result->set_error("nullpdo");
        return $result;
    }

    public function allPrepare($query, $bindArrParams = array()) {
        $result = new resultData(array(), $query, $bindArrParams);
        $result->set_error("nullpdo");
        return $result;
    }

    public function changePrepare($query, $bindArrParams = array()) {
        $result = new resultData(array(), $query, $bindArrParams);
        $result->set_error("nullpdo");
        return $result;
    }

    public function simplePrepare($query, $bindArrParams = array()) {
        $result = new resultData(array(), $query, $bindArrParams);
        $result->set_error("nullpdo");
        return $result;
    }

}

class Info implements ArrayAccess {

    function offsetSet($key, $value) {
        if (array_key_exists($key, get_object_vars($this))) {
            $this->{$key} = $value;
        }
    }

    function offsetGet($key) {
        if (array_key_exists($key, get_object_vars($this))) {
            return $this->{$key};
        }
    }

    function offsetUnset($key) {
        if (array_key_exists($key, get_object_vars($this))) {
            unset($this->{$key});
        }
    }

    function offsetExists($offset) {
        return array_key_exists($offset, get_object_vars($this));
    }

}

?>