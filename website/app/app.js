'use strict';

// Declare app level module which depends on views, and components
angular.module('myApp', [
  'duScroll',
  'ngRoute'
]).
config(['$locationProvider', '$routeProvider', function($locationProvider, $routeProvider) {
  $locationProvider.hashPrefix('!');

  $routeProvider.otherwise({redirectTo: '/view1'});
}])
.controller('mainCtrl', function($scope, $document){

});

