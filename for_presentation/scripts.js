$(document).ready(function () {
    $(function () {
        $("#regbutton").click(function (e) {
            e.preventDefault();
            var formData = {
                "login": $("#login").val()
                , "password": $("#password").val()
                , "email": $("#email").val()

            };
            $.ajax({
                url: 'auth'
                , type: 'POST'
                , data: JSON.stringify(formData)
                , success: function (res) {
                    alert(res);
                }
            });
            return false;
        });
        $("#loginbutton").click(function (e) {
            e.preventDefault();
            var formData = {
                "login": $("#login").val()
                , "password": $("#password").val()

            };
            $.ajax({
                url: 'auth',
                type: 'GET',
                data: JSON.stringify(formData),
                success: function (res) {
                    alert(res);
                }
            });
            return false;
        });
        $("#chataddbutton").click(function (e) {
            e.preventDefault();
            var formData = {
                "title": $("#chattitle").val()
                , "members_list": $("#memberlist").val()
                , "admin_list": $("#adminlist").val()

            };
            $.ajax({
                url: 'chat'
                , type: 'POST',
                xhrFields: {
                    withCredentials: true
                }
                , data: JSON.stringify(formData)
                , success: function (res) {
                    console.log()
                }
            });
            return false;
        });
    });
});