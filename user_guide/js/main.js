var Ug = function($) {

  var autoScrolling = false;

  /* window querying */

  function wideView(windowWidth) {
    var w = (typeof windowWidth === 'undefined' ? $(window).width() : windowWidth);
    return w > 787;
  }

  /* text manipulation */

  function squashedTextContent($element) {
    return $element.text().replace(/\s+/g, ' ');
  }

  /* menu and navigation - wide window */

  function scrollTo($element) {
    var targetPosition = $element.offset().top;
    autoScrolling = true;
    $('html, body').stop().animate(
      { 'scrollTop': targetPosition },
      'fast',
      function() { autoScrolling = false; }
    );
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
    var $menuItem = $link.closest('li:visible');
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
    configureForSize();
    contractMenu();
  }

  function enableScrollToTop() {
    $('.js-ug-scroll-to-top').click(function(event) {
      event.preventDefault();
      contractMenu();
      scrollTo($('body')); 
    });
  }

  function activateMenuItemsOnScroll() {
    $('h2, h3').waypoint(function(direction) {
      if (!autoScrolling) {
        selectMenuItemFor($(this));
      }
    });
  }

  /* toggling sections - for narrow window */

  function toggleMajorSection(event) {
    event.preventDefault();
    event.stopPropagation();
    $(this)
      .closest('.js-ug-headed-section-major')
      .find('.js-ug-headed-section-major-body')
      .toggle();
  }

  function configureForSize() {
    var windowWidth = $(window).width();
    var w = Math.max(windowWidth * 0.25, 200);
    if (wideView(windowWidth)) {
      $('#ug-left-sidebar, nav').width(w);
      $('#ug-left-sidebar').show();
      $('#ug-main').css({ 'margin-left': w + 20 + 'px' });
      $('.js-ug-headed-section-major-body').show();
      $('.js-ug-headed-section-major-title').each(function() {
        $(this).removeClass('clickable-title').off('click', toggleMajorSection);
      });
    } else {
      $('#ug-left-sidebar, nav').width(windowWidth);
      $('#ug-main').css({ 'margin-left': '0' });
      $('.js-ug-headed-section-major-body').hide();
      $('.js-ug-headed-section-major-title:not(.clickable-title)').each(function() {
        $(this).on('click', toggleMajorSection).addClass('clickable-title');
      });
    }
  }

  /* internal links to headed sections */

  function enableAutolinks() {
    $('body').on('click', '.js-ug-autolink', function(event) {
      var $autolink = $(this);
      var $header = $(':header').filter(function() {
        return squashedTextContent($(this)) === squashedTextContent($autolink);
      });
      $header
        .closest('.js-ug-headed-section-major')
        .find('.js-ug-headed-section-major-body')
        .slideDown({ duration: 'fast' });
      scrollTo($header);
      selectMenuItemFor($header);
    });
  }


  function initialize() {
    createMenu();
    enableAutolinks();
    enableScrollToTop();
    activateMenuItemsOnScroll();
    $(window).resize(configureForSize);
  }

  return {
    initialize: initialize
  }

}(jQuery);

$(document).ready(Ug.initialize);
