var Ug = function($) {

  var autoScrolling = false;

  function scrollTo($element) {
    autoScrolling = true;
    $('html, body').stop().animate(
      { 'scrollTop': $element.offset().top },
      'slow',
      function() { autoScrolling = false; }
    );
  }

  function squashedTextContent($element) {
    return $element.text().replace(/\s+/g, ' ');
  }

  function $createAutolinkTo($header) {
    var text = squashedTextContent($header);
    return $('<a href="#" class="js-ug-autolink">' + text + '</a>');
  }

  function contractMenu() {
    $('nav ul ul').hide();
  }

  function expandMenuItem($menuListItem) {
    $menuListItem.parent().show();
    $menuListItem.children('ul').show();
  }

  function selectMenuItemFor($header) {
    var $link = $('nav a').filter(function() {
      return squashedTextContent($(this)) === squashedTextContent($header);
    });
    var $menuItem = $link.closest('li');
    contractMenu();
    expandMenuItem($menuItem);
    $menuItem.children('a').focus();
  }

  function createMenu() {
    var $nav = $('nav');
    var $ul = $('<ul></ul>').appendTo($nav);
    $('h2').each(function(index, header) {
      var $header = $(header);
      var $listItem =
        $('<li></li>').append($createAutolinkTo($header)).appendTo($ul);
      var $subHeaders = $header.closest('.js-ug-headed-section').find('h3');
      if ($subHeaders.size() !== 0) {
        var $subList = $('<ul></ul>');
        $subHeaders.each(function(index, subHeader) {
          var $subHeader = $(subHeader);
          $('<li></li>').
            append($createAutolinkTo($subHeader)).
            appendTo($subList);
        });
        $subList.appendTo($listItem);
      }
    });
    setWidths();
    contractMenu();
  }

  function setWidths() {
    var w = Math.max($(window).width() * 0.25, 200);
    $('#ug-left-sidebar, nav').width(w);
    $('#ug-main').css({ 'margin-left': w + 20 + 'px' });

    // hide menu scrollbar
    $('#ug-left-sidebar').width($('nav').get(0).scrollWidth);
  }

  function enableScrollToTop() {
    $('.js-ug-scroll-to-top').click(function(event) {
      event.preventDefault();
      contractMenu();
      scrollTo($('body')); 
    });
  }

  function enableAutolinks() {
    $('body').on('click', '.js-ug-autolink', function(event) {
      var $autolink = $(this);
      var $header = $(':header').filter(function() {
        return squashedTextContent($(this)) === squashedTextContent($autolink);
      });
      scrollTo($header);
      selectMenuItemFor($header);
    });
  }

  function activateMenuItemsOnScroll() {
    $('h2, h3').waypoint(function(direction) {
      if (!autoScrolling) {
        selectMenuItemFor($(this));
      }
    });
  }

  function initialize() {
    createMenu();
    enableAutolinks();
    enableScrollToTop();
    activateMenuItemsOnScroll();
    $(window).resize(setWidths);
  }

  return {
    initialize: initialize
  }

}(jQuery);

$(document).ready(Ug.initialize);
