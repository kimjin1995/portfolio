var express = require('express');
var router = express.Router();
var store = require('store');
/* MySQL load */ 
var mysql = require('mysql');
var pool = mysql.createPool({
    connectionLimit: 5,
    host: 'localhost',
    user: 'root',
    database: 'miniq',
    password: 'kj1070110kj!'

});

/* Use Multer to upload imgFile */
var multer = require('multer');
var upload = multer({ 
    dest: 'public/imgFile/'
});



/* GET users listing. */
router.get('/', function (req, res, next) {
     var userid = store.get('logid').id;
     var username = store.get('logname').name;
	store.remove("move");
	store.remove("type");
    //그냥 board/ 로 접속할 경우 전체 목록 표시로 리다이렉팅
    res.redirect('/notice/page/1');
});


/* write GET */
router.get('/write', function (req, res, next) {
     var userid = store.get('logid').id;
     var username = store.get('logname').name;

    res.render('notice/write', { title: "게시판 글 쓰기" , logid : userid, logname : username});
});

/* write POST */
router.post('/write', upload.single('imgFile'), function (req, res, next) {

     var userid = store.get('logid').id;
     var username = store.get('logname').name;
	var creator_id = userid;
	var title = req.body.title;
	var content = req.body.content;


	 let file = req.file
	let result = null;
	 if(file)
	{
  	    result =file.filename
	}

	var datas = [creator_id,title,content,result];

	pool.getConnection(function (err,connection){
	var sqlForInsertBoard = "insert into noticeboard(creator_id,title,content,imgFile) values(?,?,?,?)";
	connection.query(sqlForInsertBoard ,datas, function(err,rows){
		if(err) console.error("err : " + err);
		console.log("rows : " + JSON.stringify(rows));

		res.redirect('/notice');
		connection.release();
	});
       });
});



/* read logic GET */
router.get('/read/:idx', function (req, res, next)
{
     var userid = store.get('logid').id;
     var username = store.get('logname').name;

    var idx = req.params.idx;

    pool.getConnection(function (err, connection)
    {
        var sql = "select idx, creator_id, SubName, BaseName, title, content, hit, imgFile from subject, baseinformation,noticeboard where SubSubjectNum = SubjectNum AND creator_id = BaseId AND idx = ?";
        connection.query(sql, [idx], function (err, row)
        {
            if (err) console.error(err);

	        var sql1 = "UPDATE noticeboard SET hit = hit + 1 WHERE idx = ?";
        connection.query(sql1, [idx], function (err, row1)
	{
          if (err) console.error(err);
            res.render('notice/read', { title: "글 조회", row: row[0], logid : userid, logname : username });
            connection.release();
        });
    }); 
});
});

/* update GET */
router.get('/update', function (req, res, next)
{
     var userid = store.get('logid').id;
     var username = store.get('logname').name;
    var idx = req.query.idx;

    pool.getConnection(function (err, connection)
    {
        if (err) console.error("커넥션 객체 얻어오기 에러 : ", err);

        var sql = "select idx, creator_id, title, content, hit, imgFile from noticeboard where idx=?";

        connection.query(sql, [idx], function (err, rows)
        {
	if(rows[0].creator_id == userid)
	{
            if (err) console.error(err);
            res.render('notice/update', { title: "글 수정", row: rows[0], logid : userid,logname : username });
            connection.release();
	}
	else
	{
            res.send("<script>alert('본인이 작성한 글만 수정할 수 있습니다.');history.back();</script>");
            connection.release();
	}
        });
    });
});


/* update logic POST */
router.post('/update', upload.single('imgFile'), function (req, res, next)
{
     var userid = store.get('logid').id;
     var username = store.get('logname').name;

	var idx= req.body.idx;
	var creator_id = userid;
	var title = req.body.title;
	var content = req.body.content;

	 let file = req.file
	let  imgFile= "";
	 if(file)
	{
  	     imgFile= file.filename
	}

	var datas = [creator_id,title,content, imgFile];
	
	pool.getConnection(function(err,connection)
	{
		var sql = "update noticeboard set creator_id=?, title=?, content=?, imgFile=? where idx=?";
		connection.query(sql,[creator_id,title,content,imgFile,idx],function(err,result)
		{
			console.log(result);
			if(err) console.error("글 수정 중 에러 발생 err : ",err);
		
			if(result.affectedRows==0)
			{
				res.end("<script>alert('패스워드가 일치하지 않거나, 잘못된 요청으로 인해 값이 변경되지 않았습니다.'); history.back();</script>");
			}
			else
			{
				res.redirect('notice/read/'+idx);
			}
			connection.release();
		});
	});
});




// 페이지 기능 추가
router.get('/page/:page', function (req, res, next) {  
     var userid = store.get('logid').id;
     var username = store.get('logname').name;
    pool.getConnection(function (err, connection) {
        //Use the connection

	if(store.get("move")== undefined)
	{


     

        var page = req.params.page;
        var sqlForSelectList = "select idx,creator_id,title,hit from subject, baseinformation,noticeboard where SubSubjectNum = SubjectNum AND creator_id = BaseId AND SubjectNum IN (select CourseNum from course,student where CourseId = ? AND CourseId = StudentId AND SLevel = CourseYear) order by idx desc;";
        connection.query(sqlForSelectList,userid, function (err, rows) {
            if (err) console.error("err : " + err);
	console.log(rows);
	res.render('notice/page', { title: '과목별 공지사항', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username});
            connection.release();
            //Don't use the connection here, it has been returned to the pool
     	   });
	}
else if(store.get("type").type == 1)
      {
        var temp = store.get("move").move;
	var page = req.params.page;
        var sqlForSelectList = "SELECT idx, creator_id, title, hit FROM noticeboard WHERE title Like " + "'%"  + temp +"%' order by idx desc";
	console.log(sqlForSelectList);
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));

            res.render('notice/page', { title: '과목별 공지사항', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username});
            console.log(rows.length-1);
            connection.release();
            //Don't use the connection here, it has been returned to the pool
        });
      }
 	else if(store.get("type").type == 2)
      {
        var temp = store.get("move").move;
	var page = req.params.page;
        var sqlForSelectList = "SELECT idx, creator_id, title, hit FROM noticeboard WHERE content Like " + "'%"  + temp +"%' order by idx desc";
	console.log(sqlForSelectList);
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));

             res.render('notice/page', { title: '과목별 공지사항', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username});
             console.log(rows.length-1);
            connection.release();
            //Don't use the connection here, it has been returned to the pool
        });
      }
      else
      {
        var temp = store.get("move").move;
	var page = req.params.page;
        var sqlForSelectList = "SELECT idx, creator_id, title, hit FROM noticeboard WHERE title Like " + "'%"  + temp +"%' OR content Like " + "'%"  + temp +"%' order by idx desc";
	console.log(sqlForSelectList);
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));

            res.render('notice/page', { title: '과목별 공지사항', rows: rows, page:page, length:rows.length-1, page_num:5, pass:true, logid : userid, logname : username});
              console.log(rows.length-1);
            connection.release();
            //Don't use the connection here, it has been returned to the pool
        });
      }
    });
});

router.post('/page', function (req, res, next)
{
     var userid = store.get('logid').id;
     var username = store.get('logname').name;
        var page = req.params.page;
	var x = req.body.input;
        var y = req.body.searchType;
        store.set('type',{type : y});
	store.set('move',{move : x });
	console.log(store.get("move").move);


 	   res.redirect("/notice/page/1");

});


module.exports = router;