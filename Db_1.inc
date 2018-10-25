<?php
require_once ROOT . '/inc/lib/Request.inc';
/**
 * 封装基础model类
 * 包含增删改查链表查询
 */
class Db_1 
{
    protected $dbConfig;//数据库连接配置(数组)
    
    protected  $limit;//查询限制条件（数组）

    protected $sql;//最后执行的sql语句
    
    protected $fetchSql = false; //是否返回当前执行语句
    
    protected $table; //执行的表名
    
    protected $pk;//主键
    
    protected $bindParmer = [];
    
    private   $connect;//数据库连接源
    
    /**
     * 启动器
     * @param type $dbConfig 数据库连接配置
     */
    public function __construct($dbConfig = null) 
    {
        if(empty($dbConfig)){
            $this->dbConfig = Conf::$JiaoyanDB;
        }else{
            $this->dbConfig = $dbConfig;
        }
        $this->connectDb();
        if(empty($this->table)){
            $this->table = strtolower(rtrim(get_called_class(),'DB'));
        }
        if(empty($this->pk)){
            $this->getPk();
        }
        $this->limit = array(
            '$where$' => '1=1',
            '$field$' => '*',
            '$table$' => $this->table,
            '$limit$' => '',
            '$order$' => '',
            '$group$' => '',
            '$join$'  => '',
        );
        
    }
   
    /**
     * 魔术方法用于获取指定的私有
     * @param type $name
     * @return type
     */
    public function __get($name) 
    {
        return $this->$name;
    }
    
    /**
     * 连接数据库返回连接后的数据库对象
     */
    protected function connectDb()
    {
        if(empty($this->dbConfig)){
            return FALSE;
        }
        static $instance;
        $key = $this->dbConfig["server"] . "_" . $this->dbConfig["database"] . "_" . $this->dbConfig["username"];
        if (!isset($instance[$key]) || !is_object($instance[$key])) {
            $instance[$key] = new myPDO($this->dbConfig["server"], $this->dbConfig["database"], $this->dbConfig["username"], $this->dbConfig["password"], $this->dbConfig['dbtype']);
        }
        $this->connect = $instance[$key];
        return $this;
    }
    
    /**
     * 是否返回当前执行的sql语句
     */
    public function fetchSql($bool)
    {
        $this->fetchSql = $bool;
        return $this;
    }

    /**
     * 查询当前表主键
     */
    protected function getPk()
    {
        $dbName = $this->dbConfig['database'];
        $pkSql  = "SHOW columns FROM $this->table";
        $result = $this->connect->allPrepare($pkSql);
        $field  = $result->get();
        foreach ($field as $k => $v){
            if($v['Key'] == 'PRI'){
                $this->pk = $v['Field'];
                break;
            }
        }
    }

    /**
     * 重要
     * 私有方法(过滤（1=1，1,2,3，TRUE等）)
     * 判断mysql where条件是否合法(适用于删除，更新语句)
     */
    private function jugeWhere($where)
    {
        if(stripos($where,'or') >= 0){
            $where      = strtolower($where);
            $whereArray = explode('or', $where);
            foreach ($whereArray as $value){
                if(stripos($value,'=') >= 0){
                    $ecoArray = explode('=', $value);
                    if(is_numeric($ecoArray[0])){
                        return TRUE;
                    }
                }else{
                    if(is_numeric($value) || $value === 'true'){
                        return TRUE;
                    }
                }
            }
        }else{
            if(stripos($where,'=') >= 0){
                    $ecoArray = explode('=', $where);
                    if(is_numeric($ecoArray[0])){
                        return TRUE;
                    }
                }else{
                    if(is_numeric($where) || $where === true){
                        return TRUE;
                    }
                }
        }
    }

    /**
     * @param str $needType 返回数据类型enum('array','obj');
     * 返回满足当前条件的所有数据
     */
    public function select($needType='array')
    {
       $sql = str_replace(array('$where$','$field$','$table$','$limit$','$order$','$group$','$join$'), $this->limit, 'SELECT $field$ FROM $table$ $join$ WHERE $where$ $group$ $order$ $limit$');
       $this->sql = $sql;
       if(!empty($this->fetchSql)){
           return $sql;
       }
       $result = $this->connect->allPrepare($sql,$this->bindParmer);
       if($needType == 'array'){
           list($error,$data) = array($result->error,$result->get());
           if($error){
               return $error->getMessage();
           }
           return $data;
       }
       return $result;
    }
    
    /**
     * @param int $id 主键值
     * 根据主键值获取数据
     */
    public function find(int $id)
    {
        if($id <= 0){
            return FALSE;
        }
        $findSql = str_replace('$field$', $this->limit['$field$'],' SELECT $field$ FROM '. $this->table . ' WHERE ' . "$this->pk=$id");
        if($this->fetchSql){
            return $findSql;
        }
        $result  = $this->connect->rowPrepare($findSql);
        list($error,$data) = array($result->error,$result->get());
        if($error){
            return $error->getMessage();
        }
        return $data;
    }
    
    /**
     * 分页方法
     * @param int $pageNum 每页容量(默认10)
     * @param int $page(从请求中获取不用传)
     */
    public function page(int $pageNum = 10)
    {
       $request = new Request();
       $request->shop_set_params();
       //当前请求页
       $page    = $request->params['page'] ?? 1;
       //数据总量
       $numers  = $this->count('*');
       if(empty($numers)){
           $data['data']      = [];
           $data['data']      = 0;
           $data['crentPage'] = 0;
           $data['numers']    = 0;
           $data['firstPage'] = 0;
           $data['lastPage']  = 0;
           $data['prePage']   = 0;
           $data['nextPage']  = 0;
           $data['pageNumers']= 0;
           return $data;
       }
       //总页数 最后一页
       $lastPage = $pageNumers = ceil($numers / $pageNum);
       //上一页
       $prePage  = $page - 1 <= 0 ? 1 : $page-1;
       //下一页
       $nexPage  = ($page == $lastPage ? $page : $page + 1);
       //获取数据
       $this->limit['$limit$'] = 'LIMIT' . ' ' . ($page - 1) . ',' . $pageNum;
       $data['data']      = $this->select();
       $data['crentPage'] = $page;
       $data['numers']    = $numers;
       $data['firstPage'] = 1;
       $data['lastPage']  = $lastPage;
       $data['prePage']   = $prePage;
       $data['nextPage']  = $nexPage;
       $data['pageNumers']= $pageNumers;
       return $data;
       
    }
    
    /**
     * sql 查询
     * @param string $sql sql语句
     */
    public function query($sql)
    {
        if(empty($sql)){
            return FALSE;
        }
        $result = $this->connect->simplePrepare($sql);
        list($error,$data) = array($result->error,$result->get());
        if($error){
            return $error->getMessage();
        }
        return $data;
    }
    
    /**
     * 多条数据存储
     * @param array $data 多维数组
     */
    public function saveAll($data)
    {
        $return_id  = [];
        $numerValue = count($data);
        if($numerValue >= 0){
            foreach ($data as $k=>$value){
                $return_id[$k] = $this->save($value);
            }
        }
        return $return_id;
    }


    /*******************实现聚合查询开始*******************************************/
    /**
     * 魔术方法__call
     * 用于获取聚合数据
     */
    public function __call($name, $arguments) 
    {
        $call = strtoupper($name);
        $this->limit['$field$'] = $arguments[0];
        $sql = str_replace(array('$where$','$field$','$table$','$limit$','$order$','$group$','$join$'), $this->limit, 'SELECT ' . $call .'($field$) AS NUMERS FROM $table$ $join$ WHERE $where$ $group$ $order$ $limit$');
        $result  = $this->connect->rowPrepare($sql, $this->bindParmer);
        list($error,$data) = array($result->error,$result->get());
        if($error){
            return $error->getMessage();
        }
        return $data['NUMERS'];
    }

    /*******************实现聚合查询结束*******************************************/
    
    /**
     * 删除数据
     * @param int $id 表主键值
     */
    public function deleat(int $id=0)
    {
        if ($id > 0){
            $where  = $this->pk . '=' . $id; 
        }else{
            $where  = $this->limit['$where$'];
            if(empty($where) || $this->jugeWhere($where)){
                return FALSE;
            }
        }
        $deleteSql = "DELETE FROM $this->table WHERE $where";
        $this->sql = $deleteSql;
        if($this->fetchSql){
            return $this->sql;
        }
        $result    = $this->connect->simplePrepare($deleteSql,$this->bindParmer);
        list($error,$countNum) = array($result->error,$result->get());
        if(!empty($error)){
            return $error->getMessage();
        }else{
            return $countNum;
        }
       
    }

    /**
     * 增加数据
     */
    public function save($data)
    {
        $prepare   = $this->prepare_insert_element($data);
        $sql       = "INSERT INTO $this->table ({$prepare['field']}) VALUES ({$prepare['value']}) ";
        $this->sql = $sql;
        if($this->fetchSql){
            return $this->sql;
        }
        $result    = $this->connect->insertPrepare($sql, $prepare['prepare']);
        return $this->connect->lastInsertId();
    }

    /**
     * 更新数据
     * @param array $data 要更新的数据以键值对的一维数组形式存储
     * @param int   $id   要更新表主键值
     */
    public function update($data,int $id=0)
    {
        //判断更新的数据内是否有主键字段
        $bool = array_key_exists($this->pk, $data);
        if($bool){
            $id = $data[$this->pk];
            unset($data[$this->pk]);
        }
        
        $prepare = $this->prepare_update_element($data);
        if($id > 0){
            $upSql = "UPDATE $this->table SET {$prepare['field']} WHERE $this->pk=$id";
        } else {
            $where = $this->limit['$where$'];
            if(empty($where) || $this->jugeWhere($where)){
                return FALSE;
            }
            $upSql = "UPDATE $this->table SET {$prepare['field']} WHERE $where";
        }
        $this->sql = $upSql;
        if($this->fetchSql){
            return $this->sql;
        }
        $result = $this->connect->changePrepare($upSql, array_merge($prepare['prepare'],$this->bindParmer));
        list($error,$countNum) = array($result->error,$result->get());
        if(!empty($error)){
            return $error->getMessage();
        }else{
            return $countNum;
        }
        
    }

    /**
     * 链表查询方法
     * @param string $join 关联限制条件
     * @return obj
     */
    public function join()
    {
        $list = func_get_args();
        if(!empty($list)){
            foreach ($list as $param){
                if(is_string($param)){
                    $this->limit['$join$'] = $param;
                }
            }
        }
        return $this;
    }
    
    /**
     * 处理查询条件
     * @param mixed $where 条件
     * obj
     */
    public function where($where)
    {
        $str       = $where;
        $bindPamer = [];
        if(is_array($where) && count($where) > 0){
            $str      = '';
            $spailStr = 'W_R';
            foreach ($where as $key=>$value){
                if(is_array($value)){
                    if(strtolower($value[0]) == 'in'){
                        $str .= $key . ' ' . $value[0] . ' ' . $value[1] . ' and '; 
                    }else{
                        $str .= $key . ' ' . $value[0] . ' ' . ':' . $spailStr . $key . ' and ';
                        $bindPamer[':' . $spailStr . $key] = $value[1];
                    }
                }else{
                    $str .= $key . '=' . ':' . $spailStr . $key . ' and ';
                    $bindPamer[':' . $spailStr . $key] = $value;
                }
            } 
            $str = substr($str,0,-5);//rtrim（bug）
            $this->bindParmer = $bindPamer;
        }
        $this->limit['$where$'] = $str;
        return $this; 
    }

    /**
     * 指定查询表
     * @param type $table 要查询的表名
     */
    public function table($table)
    {
        $this->limit['$table$'] = $table;
        return $this;
        
    }
    
    /**
     * 设置查询指定字段
     * @param string $fields 指定查询字段
     */
    public function field(string $fields)
    {
        if(!empty($fields)){
            $this->limit['$field$'] = $fields;
        }
        return $this; 
    }
    
    /**
     * 设置查询数据的条数
     * @param mixd $limit 取值限制条件
     */
    public function limit($limit)
    {
        if (is_int($limit)){
            $this->limit['$limit$'] = 'LIMIT ' . $limit;
        } else {
            $this->limit['$limit$'] = $limit;
        }
        
        return $this;
    }
    
    /**
     * 设置查询语句分组信息
     * @param string $group 分组限制语句
     */
    public function group($group)
    {
        if(stripos($group,'GROUP BY') >= 0){
            $this->limit['$group$'] = $group;
        }else{
            $this->limit['$group$'] = 'GROUP BY' . ' ' . $group;
        }
        return $this;
    }
    
    /**
     * 设置sql排序
     * @param str $order 排序限制
     */
    public function order($order)
    {
        if (stripos($order, 'ORDER BY') >= 0){
            $this->limit['$order$'] = $order;
        }else{
            $this->limit['$order$'] = 'ORDER BY' . ' ' . $order;
        }
        return $this;
    }        
    
    /**
     * 处理插入数据的绑定数组
     * @param type $hash array 待插入的数据
     * @return type
     */
    protected  function prepare_insert_element($hash) 
    {
        $field_str = '';
        $value_str = '';
        $prepare_array = array();
        foreach ($hash as $k => $v) {
            $field_str .= ',' . $k;
            $k = str_replace("`", '',$k);
            $value_str .= ',:' . $k;
            $prepare_array[':' . $k] = $v;
        }
        $field_str = trim($field_str, ', ');
        $value_str = trim($value_str, ', ');
        return array('field' => $field_str, 'value' => $value_str, 'prepare' => $prepare_array);
    }
    
    /**
     * 更新数据绑定参数数组
     * @param type $hash
     * @return type
     */
    protected function prepare_update_element($hash) 
    {
        $field_str = '';
        $prepare_array = array();
        foreach ($hash as $k => $v) {
            $field_str .= ',' . $k . '=:' . $k;
            $prepare_array[':' . $k] = $v;
        }
        $field_str = trim($field_str, ',');
        return array('field' => $field_str, 'prepare' => $prepare_array);
    }

   
}
