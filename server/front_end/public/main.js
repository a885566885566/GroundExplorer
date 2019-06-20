// Set the maximum number of input
const num_input = 5;
// Size of chart svg
var width = 800;
var height = 400;

var socket = io.connect('http://' + document.domain + ':' + location.port + '/front');
var svg = d3.select('svg')
    .style("width", width+'px')
    .style("height", height+'px');
var last_data
socket.on('back_update', (data)=>{
    console.log("response")
    console.log(data)
    if(data['v'] > 0.1){
        if(!last_data) last_data = data;
        var line = svg.append("line")
             .attr("x1", last_data['px'])  
             .attr("y1", last_data['py'])  
             .attr("x2", data['px'])  
             .attr("y2", data['py'])  
             .attr("stroke","red")  
             .attr("stroke-width",2)  
             .attr("marker-end","url(#arrow)");
         last_data = data;
    }
})


setInterval(()=>{
    socket.emit('front_update', {data: "hello"});
    console.log('sent')
}, 1000)


// 绘制直线 
// Add input and button into input_list
/*
var addInput = function(num){
    for(var i=0; i<num; i++){
        const name = "keyword " + i;
        const id = "keyword_"+i;
        var input_div = $('<div class="input-group input_box"><input type="text" class="form-control" id="'+id+'" placeholder="'+name+'" aria-label="'+name+'" aria-describedby="basic-addon2"></div>');
        //<div class="input-group-append"><button id="'+id+"_btn"+'" class="btn btn-outline-secondary input_btn" type="button">Add</button></div>
        input_div.appendTo('#input_list');
    }
}

var btn_click=function(e){
    //var name = e.currentTarget.id;
    var arg = ""
    for(var i=0; i<num_input; i++){
        var name = "keyword_"+i;
        var id = "#"+name;
        var text = $(id).val();
        arg = arg + text + ","
    }
    $.ajax({
        method:"get",
        url:"./keyword?item="+arg,
        success: function(data){
            console.log("Get data from keyword:");
            console.log(data);
            refresh_chart(data);
        }
    })
    alert("New keyword added: " + text);
}

addInput(num_input);
$(".input_btn").on('click', btn_click);  // Must be called after inputs been added
refresh_chart(data);
*/
