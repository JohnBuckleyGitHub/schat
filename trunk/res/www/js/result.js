var AuthState = {
  id: '',

  setError: function()
  {
    $('#spinner').hide();
    $('#alert').addClass('alert-error');
    $('#alert').show();
  },

  setSuccess: function()
  {
    $('#spinner').hide();
    $('#alert').addClass('alert-success');
    $('#alert').show();
  }
};

$(document).ready(function() {
  AuthState.id = $('body').attr('id');

  $.ajax({
    url: '/state/' + AuthState.id,
    dataType: 'json',
    success: function(data) {
      AuthState.setSuccess();
      console.log(data)
    },
    error: function() {
      AuthState.setError();
    }
  });
});