const express = require('express');
const path = require('path');
const crypto = require('crypto');
const router = express.Router();
const models = require("../models");
const session = require('express-session');
var store = require('store');

var mysql = require('mysql');
var pool = mysql.createPool({
    connectionLimit: 5,
    host: 'localhost',
    user: 'root',
    database: 'miniq',
    password: 'kj1070110kj!'
});
const app = express();


router.get('/', function(req, res, next) {
    if(req.cookies){
        console.log(req.cookies);
    }

    res.send("환영합니다 ~");
});


router.get('/signup', function(req, res, next) {

 pool.getConnection(function (err, connection)
    {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);

        var sql = "select distinct Major from major";
        connection.query(sql, function (err, rows)
        {
            if (err) console.error(err);
	console.log(rows);
            res.render('users/signup', { title: 'Express', rows : rows});
            connection.release();
        });

    });

});


router.post("/signup", async function(req,res,next){
    let body = req.body;

     var userid = req.body.userName;
    var username = req.body.name;
    var userbirth = req.body.birth;
    var useraddress = req.body.address;
    var userpersonal = req.body.open;
    var usermail = req.body.email;
    var userphonenum = req.body.phonenum;
    var usermajor = req.body.selectBox;
	
     pool.getConnection(function (err, connection)
    {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);

          var sql = "INSERT INTO baseinformation(BaseId, BaseName, BaseBirth, BaseAddress, BaseMajor, BasePublic, BasePhoneNum, BaseMail) VALUES (?,?,?,?,?,?,?,?)";
        connection.query(sql,[userid,username,userbirth,useraddress,usermajor,userpersonal,userphonenum,usermail], function (err, rows)
        {
            if (err) 
	{
	console.error(err);
             res.send("<script>alert('패스워드가 일치하지 않거나, 잘못된 아이디를 입력하였습니다.');history.back();</script>");
	}
	else
	{
     var sql1 = "INSERT INTO student(StudentId,SPollution,SLevel,SMajorAvg,SGradeAvg,STotalGrade,banned,bannedcount) VALUES (?,?,?,?,?,?,?,?)";
        connection.query(sql1,[userid,0,1,0,0,0,0,0,0], function (err, insert)
        {
	console.log(sql1,);
    let inputPassword = body.password;
    let salt = Math.round((new Date().valueOf() * Math.random())) + "";
    let hashPassword = crypto.createHash("sha512").update(inputPassword + salt).digest("hex");

    let result = models.user.create({
        loginid: body.userName,
        loginpw: hashPassword,
        salt: salt
    });

	console.log("성공");
            connection.release();
 	   res.redirect("/users/login");

	});
	}
	        });
    });

});

// 로그인 GET
router.get('/login', function(req, res, next) {
        let session = req.session;

    res.render("users/login", {
        session : session
    });
});

// 로그인 POST

router.post("/login", async function(req,res,next){
    let body = req.body;

    let result = await models.user.findOne({
        where: {
            loginid : body.userName
        }
    });
  if(result == undefined)
{
                  res.send("<script>alert('패스워드가 일치하지 않거나, 잘못된 아이디를 입력하였습니다.');history.back();</script>");
        res.redirect("/users/login");
}
  	 store.set('logid',{id : result.dataValues.loginid});
    let dbPassword = result.dataValues.loginpw;
    let inputPassword = body.password;
    let salt = result.dataValues.salt;
    let hashPassword = crypto.createHash("sha512").update(inputPassword + salt).digest("hex");
    if(dbPassword === hashPassword){
        console.log("비밀번호 일치");
        req.session = hashPassword;

        res.redirect("/users/screen");
    }
    else{
                  res.send("<script>alert('패스워드가 일치하지 않거나, 잘못된 아이디를 입력하였습니다.');history.back();</script>");
        res.redirect("/users/login");
    }
});



router.get("/logout", function(req,res,next){
        console.log("로그아웃 완료");
  res.clearCookie('sid');

 store.remove('logname');
 store.remove('logid');
store.remove('move');
store.remove('type');

store.remove('a1');
store.remove('a2');
store.remove('a3');
store.remove('a4');
store.remove('a5');
store.remove('a6');
store.remove('a7');
store.remove('a8');

store.remove('b1');
store.remove('b2');
store.remove('b3');
store.remove('b4');
store.remove('b5');
store.remove('b6');
store.remove('b7');
store.remove('b8');

store.remove('d1');
store.remove('d2');
store.remove('d3');
store.remove('d4');
store.remove('d5');
store.remove('d6');
store.remove('d7');
store.remove('d8');

store.remove('c1');
store.remove('c2');
store.remove('c3');
store.remove('c4');
store.remove('c5');
store.remove('c6');
store.remove('c7');
store.remove('c8');
 store.set('logid',{id: 0});
  res.redirect("/users/login")
})



router.get('/screen', (req, res) =>{
 pool.getConnection(function (err, connection)
    {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);

if(store.get('logid').id)
{
    
        var logid = store.get('logid').id;
        var sql =  'select BaseName from baseinformation where BaseId = ' + logid;
        connection.query(sql, function (err, rows)
        {
            if (err) 
	{
	console.error(err);
            res.render('users/login', { title: 'Express'});
	}
     	   var sql1 =  'select BaseName,CourseName,CourseNum,SubProfessor,SubClassroom, SubTime1,SubTime2,courseyear from course,subject,baseinformation where courseid='+logid+ ' and courseyear="1" and CourseNum = SubSubjectNum and SubProfessor = BaseId';
      	  connection.query(sql1, function (err, result)
     	   {
         	   if (err) 
		{
		console.error(err);
        		res.render('users/login', { title: 'Express'});
		}
	   if(result.length ==0)
		result[0] =   {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };

	   if(result.length < 8)
		for(var i = result.length-1; i < 7; i++)
			result[i+1] = {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };



     	   var sql2 =  'select BaseName,CourseName,CourseNum,SubProfessor,SubClassroom, SubTime1,SubTime2,courseyear from course,subject,baseinformation where courseid='+logid + ' and courseyear="2" and CourseNum = SubSubjectNum and SubProfessor = BaseId';
      	  connection.query(sql2, function (err, result1)
     	   {
         	   if (err) 
		{
		console.error(err);
        		res.render('users/login', { title: 'Express'});
		}

	   if(result1.length ==0)
		result1[0] =   {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };

	   if(result1.length < 8)
		for(var i = result1.length-1; i < 7; i++)
			result1[i+1] = {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };


     	   var sql3 =  'select BaseName,CourseName,CourseNum,SubProfessor,SubClassroom, SubTime1,SubTime2,courseyear from course,subject,baseinformation where courseid='+logid + ' and courseyear="3" and CourseNum = SubSubjectNum and SubProfessor = BaseId';
      	  connection.query(sql3, function (err, result2)
     	   {
         	   if (err) 
		{
		console.error(err);
        		res.render('users/login', { title: 'Express'});
		}

	   if(result2.length ==0)
		result2[0] =   {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };
	   if(result2.length < 8)
		for(var i = result2.length-1; i < 7; i++)
			result2[i+1] = {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };


     	   var sql4 =  'select BaseName,CourseName,CourseNum,SubProfessor,SubClassroom, SubTime1,SubTime2,courseyear from course,subject,baseinformation where courseid='+logid + ' and courseyear="4" and CourseNum = SubSubjectNum and SubProfessor = BaseId';
      	  connection.query(sql4, function (err, result3)
     	   {
         	   if (err) 
		{
		console.error(err);
        		res.render('users/login', { title: 'Express'});
		}
	   if(result3.length ==0)
		result3[0] =   {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };
	   if(result3.length < 8)
		for(var i = result3.length-1; i < 7; i++)
			result3[i+1] = {
 	  BaseName: 'null',
	    CourseName: 'null',
	    CourseNum: 'null',
	    SubProfessor: null,
	    SubClassroom: 'null',
	    SubTime1: 'null',
	    SubTime2: 'null',
	    courseyear: 'null'
	  };

	    store.set('logname',{name : rows[0].BaseName});
	console.log(result3);
	     var chck = 'select AttendId,AttendNum,AttendSub,AttendIdx,SLevel from attendance,student where AttendId = ? and AttendSub IN ( select CourseName from course where CourseYear = (select SLevel from student where StudentId=?) and CourseId = ?) AND StudentId=?';
                connection.query(chck,[logid,logid,logid,logid], function (err, rows1) {


	     var notice ='select distinct noticeboard.idx,SubMajor, SubName, BaseName,title,SubjectNum from student, course,subject,noticeboard, baseinformation where CourseId = ? AND CourseYear = SLevel AND CourseId = StudentId AND SubSubjectNum = CourseNum AND creator_id = SubProfessor AND CourseNum = SubjectNum AND BaseId = creator_id LIMIT 5';
                connection.query(notice,[logid,logid], function (err, notice) {
		
		if(notice.length < 5)	
		{
			if(notice.length == 0)
				notice[0] = {idx:'null',SubMajor: 'null', SubName: 'null',BaseName : 'null', title : 'null'};
			for(var i = notice.length;  i < 5; i++)
				notice[i] = {idx:'null',SubMajor: 'null', SubName: 'null',BaseName : 'null', title : 'null'};
		}	
		for(var i = 0; i < 5; i++)
		{
			if(notice[i].title.length > 8)
			{
			notice[i].title = notice[i].title.substr(0,8);		
			notice[i].title += "...";
			}
		}
        	  	  res.render('users/screen', { title: 'Express', logid : logid, rows: rows, logname : store.get('logname').name, result : result, result1 : result1, result2 : result2, result3 : result3, rows1 : rows1, notice: notice});

		   });


		   });


     	   });

     	   });

     	   });

     	   });

            connection.release();
        });

}
else
          res.render('users/login', { title: 'Express'});
    });

});


router.get('/check', function(req, res, next) {
        var logid = store.get('logid').id;

            res.render('users/check', { title: 'Express', rows : logid, logname : store.get('logname').name, logid:logid});
});




router.post('/check', async function(req,res,next){
    let body = req.body;
    var logid = store.get('logid').id;
    let result = await models.user.findOne({
        where: {
            loginid : logid
        }
    });
  if(result == undefined)
{
        res.send("<script>alert('패스워드가 일치하지 않거나, 잘못된 아이디를 입력하였습니다.');history.back();</script>");
        res.redirect("/users/check");
}
    let dbPassword = result.dataValues.loginpw;
	console.log(dbPassword);
    let inputPassword = body.password;
    let salt = result.dataValues.salt;
    let hashPassword = crypto.createHash("sha512").update(inputPassword + salt).digest("hex");
    if(dbPassword === hashPassword){
        console.log("비밀번호 일치");
        req.session = hashPassword;

      res.redirect("/users/update");
    }
    else{
        console.log("비밀번호 불일치");
        res.render('users/check', { title: 'Express', rows : logid, logname : store.get('logname').name});
    }
});




router.get('/update', function(req, res, next) {

        var logid = store.get('logid').id;
 pool.getConnection(function (err, connection)
    {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);

        var sql = "select distinct BaseMajor from baseinformation";
        connection.query(sql, function (err, rows)
        {
            if (err) console.error(err);
	console.log(rows);
	var sql1 = "select Major from major";
	connection.query(sql1,function(err,major)
	{
            if (err) console.error(err);
	console.log(major);

	var sql2 = "select * from baseinformation where BaseId = " + logid;
	connection.query(sql2,function(err,information)
	{
            if (err) console.error(err);
	var sql3 = "select * from student where StudentId = " + logid;
	connection.query(sql3,function(err,spec)
	{
            if (err) console.error(err);
	console.log(information);
            res.render('users/update', { title: 'Express', rows : rows, major : major, information : information, logid : logid,logname : store.get('logname').name, spec : spec });
	});
	});


	});


            connection.release();
        });

    });

});


router.post("/update", async function(req,res,next){
    let body = req.body;

     var userid = store.get('logid').id;
    var username = req.body.name;
    var userbirth = req.body.birth;
    var useraddress = req.body.address;
    var userpersonal = req.body.open;
    var userphonenum = req.body.phonenum;
    var usermail = req.body.email;
    var usermajor = req.body.selectBox;

     pool.getConnection(function (err, connection)
    {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);

          var sql = "INSERT INTO baseinformation(BaseId, BaseName, BaseBirth, BaseAddress, BaseMajor, BasePublic, BasePhoneNum, BaseMail) VALUES (?,?,?,?,?,?,?,?)   ON DUPLICATE KEY UPDATE BaseName = ?,BaseBirth = ?, BaseAddress = ?, BaseMajor = ?, BasePublic = ?, BasePhoneNum = ?, BaseMail = ?";

        connection.query(sql,[userid,username,userbirth,useraddress,usermajor,userpersonal,userphonenum,usermail, username,userbirth,useraddress,usermajor,userpersonal,userphonenum,usermail], function (err, rows)
        {
            if (err) 
	{
	console.error(err);
             res.send("<script>alert('패스워드가 일치하지 않거나, 잘못된 아이디를 입력하였습니다.');history.back();</script>");
	}
	else
	{
   	 let inputPassword = body.password;
  	  let salt = Math.round((new Date().valueOf() * Math.random())) + "";
  	 let hashPassword = crypto.createHash("sha512").update(inputPassword + salt).digest("hex");
	console.log("12231123" + inputPassword);
	if(inputPassword)
	{
          var sql1 = "update users set loginpw = ?, salt = ? where loginid = ?";

        connection.query(sql1,[hashPassword,salt,userid], function (err, rows1)
        {
            if (err)	console.error(err);
	else
	{
	console.log("성공");
	}
        });
	}


            connection.release();
 	   res.redirect("/users/screen");
	}
        });

    });

});


 router.get('/enroll', function(req, res, next) {


        var userid = store.get('logid').id;

      pool.getConnection(function (err, connection) {
      var sqlForSelectList = "select BaseName, BaseMajor, SMajorAVG, SGradeAVG, SLevel, MajorPrice from student B, baseinformation A ,major C where A.BaseId = B.StudentId && A.BaseMajor = C.Major";

      connection.query(sqlForSelectList,  function (err, rows){
      if (err) console.error("err : " + err);
      console.log("rows : " + JSON.stringify(rows));

      res.render('users/enroll', {title: ' --', rows: rows,logid : userid, logname : store.get('logname').name});

      connection.release();
      });
      });
      });

router.get('/grade', function(req, res, next) {


    var userid = store.get('logid').id;

  pool.getConnection(function (err, connection) {
  var sqlForSelectList = "SELECT CourseGrade, CourseId, SubSubjectNum, SubMajor, SubType, SubName, CourseYear from course A, subject B where A.coursenum=B.SubSubjectNum";
  connection.query(sqlForSelectList, function (err, rows){
    var sql2 ="select CourseId, CourseYear, AVG(CourseGrade) as avc from subject , course  Group BY CourseId, CourseYear having AVG(CourseGrade)>=0";
    connection.query(sql2,function(err,information)
      {
              if (err) console.error(err);
  if (err) console.error("err : " + err);
  console.log("rows : " + JSON.stringify(rows));

  res.render('users/grade', {title: ' --',information: information, rows: rows,logid : userid, logname : store.get('logname').name});
});
  connection.release();
  });
  });
  });



router.get('/map', function(req, res, next) {

     var userid = store.get('logid').id;
            res.render('users/map', { title: 'Express', logid : userid, logname : store.get('logname').name});
});

router.get('/introduce', function(req, res, next){

  var userid = store.get('logid').id;


pool.getConnection(function (err, connection) {
var sqlForSelectList = "SELECT BaseName,BaseMail, ProfessorLab, ProfessorExplain FROM professor A, baseinformation B WHERE A.ProfessorId = B.BaseId";
connection.query(sqlForSelectList, function (err, rows){
if (err) console.error("err : " + err);
console.log("rows : " + JSON.stringify(rows));

res.render('users/introduce', {title: ' --', rows: rows,logid : userid, logname : store.get('logname').name});

connection.release();
});
});
});

router.get('/introduce_major', function(req, res, next){

  var userid = store.get('logid').id;


pool.getConnection(function (err, connection) {
var sqlForSelectList = "SELECT Major, MajorLeader,MajorOffice, MajorExplain FROM major";
connection.query(sqlForSelectList, function (err, rows){
if (err) console.error("err : " + err);
console.log("rows : " + JSON.stringify(rows));

res.render('users/introduce_major', {title: ' --', rows: rows,logid : userid, logname : store.get('logname').name});

connection.release();
});
});
});
module.exports = router;




router.get('/introduce_school', function(req, res, next) {

     var userid = store.get('logid').id;
            res.render('users/introduce_school', { title: 'Express', logid : userid, logname : store.get('logname').name});
});





// 페이지 기능 추가
router.get('/page/:page', function (req, res, next) {  
     var userid = store.get('logid').id;
     var username = store.get('logname').name;
    pool.getConnection(function (err, connection) {
        //Use the connection
	if(store.get('cnt') == undefined)
		store.set('cnt',{cnt:0});

if(store.get('a1')== undefined)
store.set('cnt',{cnt:0});
else if(store.get('b1')== undefined)
store.set('cnt',{cnt:0});
else if(store.get('c1')== undefined)
store.set('cnt',{cnt:0});
else if(store.get('d1')== undefined)
store.set('cnt',{cnt:0});

if(store.get('a1'))
{
if(store.get('a1').a1 != "")
store.set('cnt',{cnt:1});
else
store.set('cnt',{cnt:0});
}
if(store.get('b1'))	
{
if(store.get('b1').b1 != "")
store.set('cnt',{cnt:2});
}
if(store.get('c1'))
{
if(store.get('c1').c1 != "")
store.set('cnt',{cnt:3});
}
if(store.get('d1'))
{
if(store.get('d1').d1 != "")
store.set('cnt',{cnt:4});
}


	if(store.get('a1') == undefined)
	{
		store.set('a1',{a1:""});
		store.set('a2',{a2:""});
		store.set('a3',{a3:""});
		store.set('a4',{a4:""});
		store.set('a5',{a5:""});
		store.set('a6',{a6:""});
		store.set('a7',{a7:""});
		store.set('a8',{a8:""});
	}

	console.log("hello");
	if(store.get('b1') == undefined)
	{
		store.set('b1',{b1:""});
		store.set('b2',{b2:""});
		store.set('b3',{b3:""});
		store.set('b4',{b4:""});
		store.set('b5',{b5:""});
		store.set('b6',{b6:""});
		store.set('b7',{b7:""});
		store.set('b8',{b8:""});
	}

	if(store.get('c1') == undefined)
	{
		store.set('c1',{c1:""});
		store.set('c2',{c2:""});
		store.set('c3',{c3:""});
		store.set('c4',{c4:""});
		store.set('c5',{c5:""});
		store.set('c6',{c6:""});
		store.set('c7',{c7:""});
		store.set('c8',{c8:""});
	}
	if(store.get('d1') == undefined)
	{
		store.set('d1',{d1:""});
		store.set('d2',{d2:""});
		store.set('d3',{d3:""});
		store.set('d4',{d4:""});
		store.set('d5',{d5:""});
		store.set('d6',{d6:""});
		store.set('d7',{d7:""});
		store.set('d8',{d8:""});
	}




	
	if(store.get("move")== undefined)
	{

        var page = req.params.page;
        //var sqlForSelectList = "SELECT * FROM evaluationboard";
	var sqlForSelectList = "SELECT BaseName,SubSubjectNum,SubMajor,SubClassroom,SubTime1,SubTime2,SubType,SubName FROM subject INNER JOIN  baseinformation where SubProfessor = BaseId;";
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
	res.render('users/page', { title: '수강신청', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username , cnt : store.get('cnt').cnt,
	aa1 : store.get('a1').a1,
	aa2 : store.get('a2').a2,
	aa3 : store.get('a3').a3,
	aa4 : store.get('a4').a4,
	aa5 : store.get('a5').a5,
	aa6 : store.get('a6').a6,
	aa7 : store.get('a7').a7,
	aa8 : store.get('a8').a8,

	bb1 : store.get('b1').b1,
	bb2 : store.get('b2').b2,
	bb3 : store.get('b3').b3,
	bb4 : store.get('b4').b4,
	bb5 : store.get('b5').b5,
	bb6 : store.get('b6').b6,
	bb7 : store.get('b7').b7,
	bb8 : store.get('b8').b8,

	cc1 : store.get('c1').c1,
	cc2 : store.get('c2').c2,
	cc3 : store.get('c3').c3,
	cc4 : store.get('c4').c4,
	cc5 : store.get('c5').c5,
	cc6 : store.get('c6').c6,
	cc7 : store.get('c7').c7,
	cc8 : store.get('c8').c8,

	dd1 : store.get('d1').d1,
	dd2 : store.get('d2').d2,
	dd3 : store.get('d3').d3,
	dd4 : store.get('d4').d4,
	dd5 : store.get('d5').d5,
	dd6 : store.get('d6').d6,
	dd7 : store.get('d7').d7,
	dd8 : store.get('d8').d8
});
            connection.release();
            //Don't use the connection here, it has been returned to the pool
     	   });
	}
else if(store.get("type").type == 1)
      {
        var temp = store.get("move").move;

	var page = req.params.page;
	var sqlForSelectList = "SELECT BaseName,SubSubjectNum,SubMajor,SubClassroom,SubTime1,SubTime2,SubType,SubName FROM subject INNER JOIN  baseinformation where SubProfessor = BaseId AND SubMajor Like " + "'%"  + temp +"%'";

        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));

	res.render('users/page', { title: '수강신청', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username ,cnt : store.get('cnt').cnt,
	aa1 : store.get('a1').a1,
	aa2 : store.get('a2').a2,
	aa3 : store.get('a3').a3,
	aa4 : store.get('a4').a4,
	aa5 : store.get('a5').a5,
	aa6 : store.get('a6').a6,
	aa7 : store.get('a7').a7,
	aa8 : store.get('a8').a8,

	bb1 : store.get('b1').b1,
	bb2 : store.get('b2').b2,
	bb3 : store.get('b3').b3,
	bb4 : store.get('b4').b4,
	bb5 : store.get('b5').b5,
	bb6 : store.get('b6').b6,
	bb7 : store.get('b7').b7,
	bb8 : store.get('b8').b8,

	cc1 : store.get('c1').c1,
	cc2 : store.get('c2').c2,
	cc3 : store.get('c3').c3,
	cc4 : store.get('c4').c4,
	cc5 : store.get('c5').c5,
	cc6 : store.get('c6').c6,
	cc7 : store.get('c7').c7,
	cc8 : store.get('c8').c8,

	dd1 : store.get('d1').d1,
	dd2 : store.get('d2').d2,
	dd3 : store.get('d3').d3,
	dd4 : store.get('d4').d4,
	dd5 : store.get('d5').d5,
	dd6 : store.get('d6').d6,
	dd7 : store.get('d7').d7,
	dd8 : store.get('d8').d8
});
            console.log(rows.length-1);
            connection.release();
            //Don't use the connection here, it has been returned to the pool
        });
      }
 	else if(store.get("type").type == 2)
      {
        var temp = store.get("move").move;

	var page = req.params.page;
var sqlForSelectList = "SELECT BaseName,SubSubjectNum,SubMajor,SubClassroom,SubTime1,SubTime2,SubType,SubName FROM subject INNER JOIN  baseinformation where SubProfessor = BaseId AND SubName Like " + "'%"  + temp +"%'";
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));

	res.render('users/page', { title: '수강신청', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username ,cnt : store.get('cnt').cnt,
	aa1 : store.get('a1').a1,
	aa2 : store.get('a2').a2,
	aa3 : store.get('a3').a3,
	aa4 : store.get('a4').a4,
	aa5 : store.get('a5').a5,
	aa6 : store.get('a6').a6,
	aa7 : store.get('a7').a7,
	aa8 : store.get('a8').a8,

	bb1 : store.get('b1').b1,
	bb2 : store.get('b2').b2,
	bb3 : store.get('b3').b3,
	bb4 : store.get('b4').b4,
	bb5 : store.get('b5').b5,
	bb6 : store.get('b6').b6,
	bb7 : store.get('b7').b7,
	bb8 : store.get('b8').b8,

	cc1 : store.get('c1').c1,
	cc2 : store.get('c2').c2,
	cc3 : store.get('c3').c3,
	cc4 : store.get('c4').c4,
	cc5 : store.get('c5').c5,
	cc6 : store.get('c6').c6,
	cc7 : store.get('c7').c7,
	cc8 : store.get('c8').c8,

	dd1 : store.get('d1').d1,
	dd2 : store.get('d2').d2,
	dd3 : store.get('d3').d3,
	dd4 : store.get('d4').d4,
	dd5 : store.get('d5').d5,
	dd6 : store.get('d6').d6,
	dd7 : store.get('d7').d7,
	dd8 : store.get('d8').d8
});
             console.log(rows.length-1);
            connection.release();
            //Don't use the connection here, it has been returned to the pool
        });
      }
else if(store.get("type").type == 3)
      {
        var temp = store.get("move").move;

	var page = req.params.page;
var sqlForSelectList = "SELECT BaseName,SubSubjectNum,SubMajor,SubClassroom,SubTime1,SubTime2,SubType,SubName FROM subject INNER JOIN  baseinformation where SubProfessor = BaseId AND BaseName Like " + "'%"  + temp +"%'";
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
   
	res.render('users/page', { title: '수강신청', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username ,cnt : store.get('cnt').cnt,
	aa1 : store.get('a1').a1,
	aa2 : store.get('a2').a2,
	aa3 : store.get('a3').a3,
	aa4 : store.get('a4').a4,
	aa5 : store.get('a5').a5,
	aa6 : store.get('a6').a6,
	aa7 : store.get('a7').a7,
	aa8 : store.get('a8').a8,

	bb1 : store.get('b1').b1,
	bb2 : store.get('b2').b2,
	bb3 : store.get('b3').b3,
	bb4 : store.get('b4').b4,
	bb5 : store.get('b5').b5,
	bb6 : store.get('b6').b6,
	bb7 : store.get('b7').b7,
	bb8 : store.get('b8').b8,

	cc1 : store.get('c1').c1,
	cc2 : store.get('c2').c2,
	cc3 : store.get('c3').c3,
	cc4 : store.get('c4').c4,
	cc5 : store.get('c5').c5,
	cc6 : store.get('c6').c6,
	cc7 : store.get('c7').c7,
	cc8 : store.get('c8').c8,

	dd1 : store.get('d1').d1,
	dd2 : store.get('d2').d2,
	dd3 : store.get('d3').d3,
	dd4 : store.get('d4').d4,
	dd5 : store.get('d5').d5,
	dd6 : store.get('d6').d6,
	dd7 : store.get('d7').d7,
	dd8 : store.get('d8').d8
});
             console.log(rows.length-1);
            connection.release();
            //Don't use the connection here, it has been returned to the pool
        });
      }

    });
});

router.post('/decide', function (req, res, next)
{
     var userid = store.get('logid').id;
     var username = store.get('logname').name;

if(store.get('a1'))
{
if(store.get('a1').a1 != "")
store.set('cnt',{cnt:1});
else
store.set('cnt',{cnt:0});
}
if(store.get('b1'))	
{
if(store.get('b1').b1 != "")
store.set('cnt',{cnt:2});
}
if(store.get('c1'))
{
if(store.get('c1').c1 != "")
store.set('cnt',{cnt:3});
}
if(store.get('d1'))
{
if(store.get('d1').d1 != "")
store.set('cnt',{cnt:4});
}


 pool.getConnection(function (err, connection)
    {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);
	var sql = "select SLevel from student where StudentId = ?";
        connection.query(sql,[userid ], function (err, rows)
        {
            if (err) 
	{
		res.send("<script>alert('같은 과목을 수강할 수 없습니다.');history.back();</script>");
		console.error(err + 1);
	}
	if(store.get('cnt').cnt == 4)
	{
        var sql1 = "insert into course(CourseId,CourseName,CourseNum,CourseYear)  VALUES (?,?,?,?),(?,?,?,?),(?,?,?,?),(?,?,?,?)";
	  var result = "insert into attendance(AttendId,AttendNum,AttendSub) VALUES(?,?,?),(?,?,?),(?,?,?),(?,?,?)";
	}
	if(store.get('cnt').cnt == 3)
	{
        var sql1 = "insert into course(CourseId,CourseName,CourseNum,CourseYear)  VALUES (?,?,?,?),(?,?,?,?),(?,?,?,?)";
	  var result = "insert into attendance(AttendIdAttendNum,AttendSub) VALUES(?,?,?),(?,?,?),(?,?,?)";
	}
	if(store.get('cnt').cnt == 2)
	{
        var sql1 = "insert into course(CourseId,CourseName,CourseNum,CourseYear)  VALUES (?,?,?,?),(?,?,?,?)";
	  var result = "insert into attendance(AttendId,AttendNum,AttendSub) VALUES(?,?,?),(?,?,?)";
	}
	if(store.get('cnt').cnt == 1)
	{
        var sql1 = "insert into course(CourseId,CourseName,CourseNum,CourseYear)  VALUES (?,?,?,?)";
	  var result = "insert into attendance(AttendId,AttendNum,AttendSub) VALUES(?,?,?)";
	}
           connection.query(sql1,[userid,store.get('a2').a2,store.get('a1').a1,rows[0].SLevel,userid,store.get('b2').b2,store.get('b1').b1,rows[0].SLevel,userid,store.get('c2').c2,store.get('c1').c1,rows[0].SLevel,userid,store.get('d2').d2,store.get('d1').d1,rows[0].SLevel], function (err, rows)
        {
            if (err) 
	{
	   res.send("<script>alert('같은 과목을 수강할 수 없습니다.');history.back();</script>");
	console.error(err);
	}
	else
	{
	    connection.query(result,[userid, 0 , store.get('a2').a2,userid, 0 , store.get('b2').b2,userid, 0 , store.get('c2').c2,userid, 0 , store.get('d2').d2] ,function (err, rows1){
	    if (err) console.error("err : " + err);

	    connection.release();
	    });
	   res.redirect("/users/page/1");
	}

        });
        });

    });



});


router.post('/page', function (req, res, next)
{
     var userid = store.get('logid').id;
     var username = store.get('logname').name;
        var page = req.params.page;
	
	var a1 = req.body.a1;
	var a2 = req.body.a2;
	var a3 = req.body.a3;
	var a4 = req.body.a4;
	var a5 = req.body.a5;
	var a6 = req.body.a6;
	var a7 = req.body.a7;
	var a8 = req.body.a8;

	var b1 = req.body.b1;
	var b2 = req.body.b2;
	var b3 = req.body.b3;
	var b4 = req.body.b4;
	var b5 = req.body.b5;
	var b6 = req.body.b6;
	var b7 = req.body.b7;
	var b8 = req.body.b8;

	var c1 = req.body.c1;
	var c2 = req.body.c2;
	var c3 = req.body.c3;
	var c4 = req.body.c4;
	var c5 = req.body.c5;
	var c6 = req.body.c6;
	var c7 = req.body.c7;
	var c8 = req.body.c8;

	var d1 = req.body.d1;
	var d2 = req.body.d2;
	var d3 = req.body.d3;
	var d4 = req.body.d4;
	var d5 = req.body.d5;
	var d6 = req.body.d6;
	var d7 = req.body.d7;
	var d8 = req.body.d8;

	store.set('a1',{a1:a1});
	store.set('a2',{a2:a2});
	store.set('a3',{a3:a3});
	store.set('a4',{a4:a4});
	store.set('a5',{a5:a5});
	store.set('a6',{a6:a6});
	store.set('a7',{a7:a7});
	store.set('a8',{a8:a8});

	store.set('b1',{b1:b1});
	store.set('b2',{b2:b2});
	store.set('b3',{b3:b3});
	store.set('b4',{b4:b4});
	store.set('b5',{b5:b5});
	store.set('b6',{b6:b6});
	store.set('b7',{b7:b7});
	store.set('b8',{b8:b8});

	store.set('c1',{c1:c1});
	store.set('c2',{c2:c2});
	store.set('c3',{c3:c3});
	store.set('c4',{c4:c4});
	store.set('c5',{c5:c5});
	store.set('c6',{c6:c6});
	store.set('c7',{c7:c7});
	store.set('c8',{c8:c8});

	store.set('d1',{d1:d1});
	store.set('d2',{d2:d2});
	store.set('d3',{d3:d3});
	store.set('d4',{d4:d4});
	store.set('d5',{d5:d5});
	store.set('d6',{d6:d6});
	store.set('d7',{d7:d7});
	store.set('d8',{d8:d8});

        var x = req.body.input;
        var z = req.body.id;
	if(z != undefined)
	        store.set('type',{type : z});
	if(x != undefined)
		store.set('move',{move : x });


 	   res.redirect("/users/page/1");

});
router.get('/race', function(req, res, next) {


      var userid = store.get('logid').id;

    pool.getConnection(function (err, connection) {
    var sqlForSelectList = "select BaseName, BaseMajor, SMajorAVG, SGradeAVG, SLevel from student B, baseinformation A where A.BaseId = B.StudentId";
    connection.query(sqlForSelectList, function (err, rows){
    if (err) console.error("err : " + err);
    console.log("rows : " + JSON.stringify(rows));

    res.render('users/race', {title: ' --', rows: rows,logid : userid, logname : store.get('logname').name});

    connection.release();
    });
    });
    });
router.get('/scholarship', function(req, res, next) {


        var userid = store.get('logid').id;

      pool.getConnection(function (err, connection) {
      var sqlForSelectList = "select BaseName, BaseMajor, SMajorAVG, SGradeAVG, SLevel, MajorPrice from student B, baseinformation A ,major C where A.BaseId = B.StudentId && A.BaseMajor = C.Major";
      connection.query(sqlForSelectList, function (err, rows){
      if (err) console.error("err : " + err);
      console.log("rows : " + JSON.stringify(rows));

      res.render('users/scholarship', {title: ' --', rows: rows,logid : userid, logname : store.get('logname').name});

      connection.release();
      });
      });
      });

router.get('/attend', (req, res) => {
    pool.getConnection(function (err, connection) {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);
        var logid = store.get('logid').id;
        if (store.get('logid').id) {

            var logid = store.get('logid').id;
            var sql = 'select AttendId, AttendNum, AttendSub from attendance where AttendId = ' + logid;
            var sql = 'select AttendId, AttendNum, AttendSub from attendance where AttendId = ? and AttendSub IN ( select CourseName from course where CourseYear = (select SLevel from student where StudentId=?) and CourseId = ?) ;';
            connection.query(sql, [logid, logid, logid, logid], function (err, rows) {
                res.render('users/attend', { title: 'Express', rows: rows, logid: logid, logname: store.get('logname').name });
            });
        }
        else
            res.render('users/login', { title: 'Express' });

        if (store.get("subject") != undefined) {
            //Use the connection
            console.log(store.get("subject").subject);
            console.log(store.get('logid').id);
            var chk = store.get("subject").subject;
            var logid = store.get('logid').id;
            var update = 'UPDATE attendance SET AttendNum = AttendNum + 1 WHERE AttendId = ? and AttendSub = ?';
            connection.query(update, [logid, chk], function (err, rows) {
                if (err) console.error("err : " + err);
                console.log("rows : " + JSON.stringify(rows));

                connection.release();
                //Don't use the connection here, it has been returned to the pool
            });
        }
    });
    var logid = store.get('logid').id;

});


router.post('/attend', function (req, res, next) {

    var x = req.body.input2;
    store.set('subject', { subject: x });
    console.log(store.get("subject").subject);
    res.redirect("/users/attend");
});

module.exports = router;